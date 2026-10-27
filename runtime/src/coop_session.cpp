#include "galaxy/coop_session.h"

#include "galaxy/coop_menu.h"
#include "galaxy/net/bytes.h"
#include "galaxy/net/direct_transport.h"

#include <algorithm>
#include <chrono>
#include <map>
#include <utility>

namespace galaxy::coop {

namespace {

using net::ByteReader;
using net::ByteWriter;
using net::Channel;
using net::PeerId;
using Clock = std::chrono::steady_clock;

// The session's messages: a type, then its body.
enum Message : std::uint8_t {
    kJoinRequest = 1,   // joiner: the session protocol, the game's version, the player's name and character
    kJoinAccept = 2,    // host: the joiner's number, the room, the save, everyone in the room
    kJoinRefuse = 3,    // host: why
    kPlayerInfo = 4,    // host: a player who joined (or changed)
    kRoomSettings = 5,  // host: the room's settings changed
    kPlayerPlace = 6,   // anyone, to everyone: where the sender plays now (its stage and scenario)
};

// A joiner that doesn't ask to be let in this soon is removed; a join that isn't let in this soon fails.
constexpr auto kJoinRequestTimeout = std::chrono::seconds(15);
constexpr auto kJoinTimeout = std::chrono::seconds(25);
// A refused joiner stays this long so the refusal reaches it before the host removes it.
constexpr auto kRefuseLinger = std::chrono::seconds(1);
constexpr auto kTick = std::chrono::milliseconds(2);
constexpr auto kIdleTick = std::chrono::milliseconds(20);
constexpr std::size_t kMaxSaveFiles = 64u;
// A stage's name: the game's are at most 47 characters (NebulaCoopPlayer).
constexpr std::size_t kStageNameLimit = 47u;

// A stage's name as the game names them (letters, digits and underscores, cut): nothing else reaches the game.
std::string clean_stage(std::string_view stage) {
    std::string out;
    for (const char c : stage) {
        const bool plain = (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9') || c == '_';
        if (!plain || out.size() >= kStageNameLimit) return {};
        out += c;
    }
    return out;
}

void write_room(ByteWriter& out, const RuntimeCoopSettings& room) {
    out.u8(static_cast<std::uint8_t>(room.player_interaction))
        .u8(static_cast<std::uint8_t>(std::min(room.knockback_strength, 255u)))
        .u8(static_cast<std::uint8_t>(room.pvp_mode))
        .u8(room.cheats ? 1u : 0u)
        .u8(static_cast<std::uint8_t>(room.max_players));
}

// Into `room`, keeping what isn't the room's (the player's name and character, the address).
void read_room(ByteReader& in, RuntimeCoopSettings& room) {
    const unsigned interaction = in.u8();
    room.player_interaction = static_cast<CoopPlayerInteraction>(std::min(interaction, 2u));
    room.knockback_strength = in.u8();
    room.pvp_mode = static_cast<CoopPvpMode>(std::min<unsigned>(in.u8(), 1u));
    room.cheats = in.u8() != 0u;
    room.max_players = std::clamp<unsigned>(in.u8(), 2u, kCoopMaxPlayersLimit);
}

void write_player(ByteWriter& out, const SessionPlayer& player) {
    out.u64(player.id)
        .u8(static_cast<std::uint8_t>(player.index))
        .str(player.name)
        .u8(static_cast<std::uint8_t>(player.character))
        .u8(player.host ? 1u : 0u)
        .str(player.stage)
        .u8(static_cast<std::uint8_t>(std::clamp(player.scenario, 0, 255)));
}

SessionPlayer read_player(ByteReader& in) {
    SessionPlayer player;
    player.id = in.u64();
    player.index = in.u8();
    player.name = in.str(kPlayerNameLimit);
    player.character = in.u8() == 1u ? CoopCharacter::Luigi : CoopCharacter::Mario;
    player.host = in.u8() != 0u;
    player.stage = clean_stage(in.str(256u));
    player.scenario = in.u8();
    return player;
}

// A name as the others see it: printable, cut, never empty.
std::string clean_name(std::string_view name) {
    std::string out;
    for (const char c : name) {
        if (c >= 0x20 && c < 0x7f && out.size() < kPlayerNameLimit) out += c;
    }
    while (!out.empty() && out.back() == ' ') out.pop_back();
    return out.empty() ? std::string("PLAYER") : out;
}

}  // namespace

struct Session::State {
    std::unique_ptr<net::Transport> transport;
    SessionState state = SessionState::Idle;
    bool host = false;
    HostConfig host_config;
    JoinConfig join_config;
    RuntimeCoopSettings room;
    std::vector<SessionPlayer> players;
    // A host's members not let in yet (since when), and refused ones to remove (when).
    std::map<PeerId, Clock::time_point> waiting;
    std::map<PeerId, Clock::time_point> refused;
    Clock::time_point join_started{};
    std::vector<SessionEvent> events;
    std::optional<RoomSave> save;
    std::map<std::uint8_t, Handler> handlers;
    // Where this player plays (set_place), for its entry in a room opened or joined later.
    std::string stage;
    int scenario = 0;

    SessionPlayer* player(PeerId id) {
        const auto it = std::find_if(players.begin(), players.end(), [id](const auto& p) { return p.id == id; });
        return it == players.end() ? nullptr : &*it;
    }

    void event(SessionEvent::Kind kind, std::string name = {}, std::string text = {}) {
        events.push_back({kind, std::move(name), std::move(text)});
    }

    bool send(PeerId to, std::uint8_t type, const ByteWriter& body, Channel channel = Channel::Ordered) {
        ByteWriter out;
        out.u8(type).raw(body.bytes());
        return transport->send_to(to, channel, out.bytes());
    }

    // The room ends for this player: back to idle, `kind` with `why`.
    void end(SessionEvent::Kind kind, std::string why) {
        if (transport->in_lobby() || state != SessionState::Idle) transport->lobby_leave();
        state = SessionState::Idle;
        host = false;
        players.clear();
        waiting.clear();
        refused.clear();
        event(kind, {}, std::move(why));
    }

    void refuse(PeerId peer, std::string why) {
        ByteWriter body;
        body.str(why);
        send(peer, kJoinRefuse, body);
        waiting.erase(peer);
        refused[peer] = Clock::now();
    }

    void host_receive_join(PeerId from, ByteReader& in) {
        if (player(from) != nullptr || refused.count(from) != 0u) return;
        const std::uint16_t protocol = in.u16();
        const std::string version = in.str(128u);
        const std::string name = clean_name(in.str(256u));
        const auto character = in.u8() == 1u ? CoopCharacter::Luigi : CoopCharacter::Mario;
        if (!in.ok() || protocol != kSessionProtocol) {
            refuse(from, "THE HOST PLAYS ANOTHER VERSION OF ONLINE PLAY");
            return;
        }
        if (version != host_config.game_version) {
            refuse(from, "THE HOST PLAYS ANOTHER VERSION OF NEBULA (" + host_config.game_version + ")");
            return;
        }
        if (players.size() >= room.max_players) {
            refuse(from, "THE ROOM IS FULL");
            return;
        }
        // The lowest free number.
        unsigned index = 1u;
        while (std::any_of(players.begin(), players.end(), [index](const auto& p) { return p.index == index; })) {
            ++index;
        }
        SessionPlayer joined{from, index, name, character, false, false, {}, 0};
        waiting.erase(from);

        RoomSave room_save;
        if (host_config.save) room_save = host_config.save();
        std::size_t total = 0u;
        for (const SaveFile& file : room_save.files) total += file.bytes.size();
        if (total > kMaxSaveBytes || room_save.files.size() > kMaxSaveFiles) room_save.files.clear();

        // Everyone already in hears of the new player, then the new player gets the room.
        ByteWriter info;
        write_player(info, joined);
        for (const SessionPlayer& other : players) {
            if (!other.local) send(other.id, kPlayerInfo, info);
        }
        players.push_back(joined);
        std::sort(players.begin(), players.end(), [](const auto& a, const auto& b) { return a.index < b.index; });

        ByteWriter accept;
        accept.u8(static_cast<std::uint8_t>(index));
        write_room(accept, room);
        accept.u8(static_cast<std::uint8_t>(room_save.file)).u16(static_cast<std::uint16_t>(room_save.files.size()));
        for (const SaveFile& file : room_save.files) accept.str(file.name).blob(file.bytes);
        accept.u8(static_cast<std::uint8_t>(players.size()));
        for (const SessionPlayer& player : players) write_player(accept, player);
        send(from, kJoinAccept, accept);
        event(SessionEvent::Kind::PlayerJoined, name);
    }

    void join_receive_accept(ByteReader& in) {
        if (state != SessionState::Connecting) return;
        (void)in.u8();  // this player's number, which the list below has too
        RuntimeCoopSettings got = room;
        read_room(in, got);
        RoomSave room_save;
        room_save.file = std::clamp<unsigned>(in.u8(), 1u, 6u);
        const unsigned files = in.u16();
        for (unsigned i = 0; i < files && in.ok() && i < kMaxSaveFiles; ++i) {
            SaveFile file;
            file.name = in.str(128u);
            file.bytes = in.blob(kMaxSaveBytes);
            room_save.files.push_back(std::move(file));
        }
        const unsigned count = in.u8();
        std::vector<SessionPlayer> list;
        for (unsigned i = 0; i < count && in.ok(); ++i) list.push_back(read_player(in));
        if (!in.ok()) {
            end(SessionEvent::Kind::JoinFailed, "THE HOST'S ANSWER COULD NOT BE READ");
            return;
        }
        for (SessionPlayer& player : list) {
            player.local = player.id == transport->self();
            if (player.local) {
                player.stage = stage;
                player.scenario = scenario;
            }
        }
        players = std::move(list);
        room = got;
        save = std::move(room_save);
        state = SessionState::Joined;
        send_place();
        event(SessionEvent::Kind::Joined);
    }

    // This player's place to everyone in the room (a host's members still joining get it with the room).
    void send_place() {
        ByteWriter body;
        body.str(stage).u8(static_cast<std::uint8_t>(std::clamp(scenario, 0, 255)));
        for (const SessionPlayer& other : players) {
            if (!other.local) send(other.id, kPlayerPlace, body);
        }
    }

    void receive_place(PeerId from, ByteReader& in) {
        std::string got = clean_stage(in.str(256u));
        const int got_scenario = in.u8();
        SessionPlayer* sender = player(from);
        if (!in.ok() || sender == nullptr || sender->local) return;
        sender->stage = std::move(got);
        sender->scenario = got_scenario;
    }

    void receive(PeerId from, Channel channel, std::span<const std::uint8_t> data) {
        ByteReader in(data);
        const std::uint8_t type = in.u8();
        if (!in.ok()) return;
        if (type == kPlayerPlace) {
            // From any player let in (passed on by the host), once this one is in.
            if (state == SessionState::Hosting || state == SessionState::Joined) receive_place(from, in);
            return;
        }
        if (type >= kFirstFeatureMessage) {
            // Only from players let in, and only once this one is in.
            const SessionPlayer* sender = player(from);
            if (sender == nullptr || state == SessionState::Connecting) return;
            const auto it = handlers.find(type);
            if (it != handlers.end() && it->second) {
                const SessionPlayer copy = *sender;
                it->second(copy, in.rest());
            }
            return;
        }
        (void)channel;
        if (host) {
            if (type == kJoinRequest) host_receive_join(from, in);
            return;
        }
        if (from != transport->owner()) return;
        switch (type) {
        case kJoinAccept: join_receive_accept(in); return;
        case kJoinRefuse: {
            const std::string why = in.str(256u);
            end(SessionEvent::Kind::JoinFailed, why.empty() ? std::string("THE HOST REFUSED") : why);
            return;
        }
        case kPlayerInfo: {
            SessionPlayer joined = read_player(in);
            if (!in.ok() || state != SessionState::Joined) return;
            joined.local = joined.id == transport->self();
            if (SessionPlayer* known = player(joined.id)) {
                // Its place comes from the player itself (kPlayerPlace), never older from the host.
                joined.stage = known->stage;
                joined.scenario = known->scenario;
                *known = joined;
                return;
            }
            players.push_back(joined);
            std::sort(players.begin(), players.end(), [](const auto& a, const auto& b) { return a.index < b.index; });
            event(SessionEvent::Kind::PlayerJoined, joined.name);
            return;
        }
        case kRoomSettings: {
            RuntimeCoopSettings got = room;
            read_room(in, got);
            if (!in.ok()) return;
            room = got;
            event(SessionEvent::Kind::RoomChanged);
            return;
        }
        default: return;
        }
    }

    void peer_left(PeerId peer, bool intentional) {
        waiting.erase(peer);
        refused.erase(peer);
        const auto it = std::find_if(players.begin(), players.end(), [peer](const auto& p) { return p.id == peer; });
        if (it == players.end()) return;
        const std::string name = it->name;
        players.erase(it);
        event(SessionEvent::Kind::PlayerLeft, name, intentional ? std::string() : std::string("STOPPED ANSWERING"));
    }

    void error(net::NetError error, PeerId) {
        if (error == net::NetError::PeerFailed && transport->in_lobby()) return;  // a member; peer_left follows
        std::string why = net::net_error_text(error);
        for (char& c : why) c = static_cast<char>(c >= 'a' && c <= 'z' ? c - 'a' + 'A' : c);
        if (state == SessionState::Connecting) {
            end(SessionEvent::Kind::JoinFailed, std::move(why));
        } else if (state != SessionState::Idle) {
            end(SessionEvent::Kind::Closed, std::move(why));
        }
    }

    void tick() {
        if (state == SessionState::Idle) return;
        transport->update();
        if (state == SessionState::Idle) return;
        const Clock::time_point now = Clock::now();
        if (state == SessionState::Connecting && now - join_started > kJoinTimeout) {
            end(SessionEvent::Kind::JoinFailed, "THE HOST DID NOT LET YOU IN");
            return;
        }
        if (!host) return;
        std::vector<PeerId> remove;
        for (const auto& [peer, since] : waiting) {
            if (now - since > kJoinRequestTimeout) remove.push_back(peer);
        }
        for (const auto& [peer, since] : refused) {
            if (now - since > kRefuseLinger) remove.push_back(peer);
        }
        for (const PeerId peer : remove) {
            waiting.erase(peer);
            refused.erase(peer);
            transport->unpeer(peer);
        }
    }
};

Session::Session(std::unique_ptr<net::Transport> transport) : state_(std::make_unique<State>()) {
    state_->transport = transport ? std::move(transport) : std::make_unique<net::DirectTransport>();
    net::TransportCallbacks callbacks;
    State* s = state_.get();
    callbacks.on_lobby_joined = [s](PeerId self, PeerId owner, std::uint64_t) {
        if (s->host) {
            s->state = SessionState::Hosting;
            s->players = {SessionPlayer{self, 0u, clean_name(s->host_config.name), s->host_config.character, true,
                                        true, s->stage, s->scenario}};
            s->event(SessionEvent::Kind::Hosting, {}, std::to_string(s->transport->port()));
            return;
        }
        ByteWriter body;
        body.u16(kSessionProtocol)
            .str(s->join_config.game_version)
            .str(clean_name(s->join_config.name))
            .u8(static_cast<std::uint8_t>(s->join_config.character));
        s->send(owner, kJoinRequest, body);
    };
    callbacks.on_peer_joined = [s](PeerId peer) {
        if (s->host) s->waiting[peer] = Clock::now();
    };
    callbacks.on_peer_left = [s](PeerId peer, bool intentional) { s->peer_left(peer, intentional); };
    callbacks.on_receive = [s](PeerId from, Channel channel, std::span<const std::uint8_t> data) {
        s->receive(from, channel, data);
    };
    callbacks.on_error = [s](net::NetError error, PeerId tag) { s->error(error, tag); };
    state_->transport->set_callbacks(std::move(callbacks));
}

Session::~Session() {
    leave();
    stop_thread();
}

void Session::start_thread() {
    if (running_.exchange(true)) return;
    thread_ = std::thread([this] { run(); });
}

void Session::stop_thread() {
    running_.store(false);
    if (thread_.joinable()) thread_.join();
}

void Session::run() {
    while (running_.load(std::memory_order_relaxed)) {
        bool idle = false;
        {
            const std::lock_guard<std::recursive_mutex> lock(lock_);
            state_->tick();
            idle = state_->state == SessionState::Idle;
        }
        std::this_thread::sleep_for(idle ? kIdleTick : kTick);
    }
}

bool Session::host(HostConfig config) {
    {
        const std::lock_guard<std::recursive_mutex> lock(lock_);
        if (state_->state != SessionState::Idle) state_->end(SessionEvent::Kind::Closed, {});
        state_->events.clear();
        state_->host = true;
        state_->host_config = std::move(config);
        state_->room = state_->host_config.room;
        state_->room.max_players = std::clamp(state_->room.max_players, 2u, kCoopMaxPlayersLimit);
        net::LobbyConfig lobby;
        lobby.port = state_->host_config.port;
        lobby.game = "Nebula";
        lobby.version = state_->host_config.game_version;
        lobby.host_name = state_->host_config.name;
        lobby.password = state_->host_config.room.password;
        lobby.max_members = state_->room.max_players;
        if (!state_->transport->lobby_create(lobby)) {
            state_->host = false;
            state_->state = SessionState::Idle;
            return false;
        }
    }
    start_thread();
    return true;
}

bool Session::join(JoinConfig config) {
    {
        const std::lock_guard<std::recursive_mutex> lock(lock_);
        if (state_->state != SessionState::Idle) state_->end(SessionEvent::Kind::Closed, {});
        state_->events.clear();
        state_->host = false;
        state_->join_config = std::move(config);
        state_->room = RuntimeCoopSettings{};
        state_->save.reset();
        state_->state = SessionState::Connecting;
        state_->join_started = Clock::now();
        net::JoinConfig lobby{state_->join_config.address, state_->join_config.port, state_->join_config.password};
        if (!state_->transport->lobby_join(lobby)) {
            // on_error already told why (JoinFailed).
            state_->state = SessionState::Idle;
            return false;
        }
    }
    start_thread();
    return true;
}

void Session::leave() {
    const std::lock_guard<std::recursive_mutex> lock(lock_);
    if (state_->state == SessionState::Idle) return;
    state_->end(SessionEvent::Kind::Closed, {});
}

SessionState Session::state() const {
    const std::lock_guard<std::recursive_mutex> lock(lock_);
    return state_->state;
}

bool Session::is_host() const {
    const std::lock_guard<std::recursive_mutex> lock(lock_);
    return state_->host && state_->state == SessionState::Hosting;
}

std::vector<SessionPlayer> Session::players() const {
    const std::lock_guard<std::recursive_mutex> lock(lock_);
    std::vector<SessionPlayer> players = state_->players;
    for (SessionPlayer& player : players) {
        if (!player.local) player.ping_ms = static_cast<unsigned>(state_->transport->ping(player.id).count());
    }
    return players;
}

RuntimeCoopSettings Session::room() const {
    const std::lock_guard<std::recursive_mutex> lock(lock_);
    return state_->room;
}

std::uint16_t Session::port() const {
    const std::lock_guard<std::recursive_mutex> lock(lock_);
    return state_->transport->in_lobby() && state_->host ? state_->transport->port() : 0u;
}

void Session::set_room(const RuntimeCoopSettings& room) {
    const std::lock_guard<std::recursive_mutex> lock(lock_);
    if (state_->state != SessionState::Hosting) return;
    const unsigned max_players = state_->room.max_players;  // the lobby's size is set as it opens
    RuntimeCoopSettings next = state_->room;
    next.player_interaction = room.player_interaction;
    next.knockback_strength = room.knockback_strength;
    next.pvp_mode = room.pvp_mode;
    next.cheats = room.cheats;
    next.max_players = max_players;
    if (next == state_->room) return;
    state_->room = next;
    ByteWriter body;
    write_room(body, next);
    for (const SessionPlayer& player : state_->players) {
        if (!player.local) state_->send(player.id, kRoomSettings, body);
    }
}

void Session::set_place(const std::string& stage, int scenario) {
    const std::lock_guard<std::recursive_mutex> lock(lock_);
    const std::string clean = clean_stage(stage);
    scenario = std::clamp(scenario, 0, 255);
    if (clean == state_->stage && scenario == state_->scenario) return;
    state_->stage = clean;
    state_->scenario = scenario;
    if (state_->state != SessionState::Hosting && state_->state != SessionState::Joined) return;
    for (SessionPlayer& player : state_->players) {
        if (player.local) {
            player.stage = clean;
            player.scenario = scenario;
        }
    }
    state_->send_place();
}

std::vector<SessionEvent> Session::take_events() {
    const std::lock_guard<std::recursive_mutex> lock(lock_);
    return std::exchange(state_->events, {});
}

std::optional<RoomSave> Session::take_room_save() {
    const std::lock_guard<std::recursive_mutex> lock(lock_);
    return std::exchange(state_->save, std::nullopt);
}

bool Session::send(std::uint8_t type, net::Channel channel, std::span<const std::uint8_t> data, net::PeerId to) {
    if (type < kFirstFeatureMessage) return false;
    const std::lock_guard<std::recursive_mutex> lock(lock_);
    if (state_->state != SessionState::Hosting && state_->state != SessionState::Joined) return false;
    ByteWriter out;
    out.u8(type).raw(data);
    if (to != net::kEveryone) return state_->transport->send_to(to, channel, out.bytes());
    // Everyone let in (a host's members still joining don't get the room's messages yet).
    if (!state_->host) return state_->transport->send_to(net::kEveryone, channel, out.bytes());
    bool sent = true;
    for (const SessionPlayer& player : state_->players) {
        if (!player.local) sent = state_->transport->send_to(player.id, channel, out.bytes()) && sent;
    }
    return sent;
}

void Session::on_message(std::uint8_t type, Handler handler) {
    const std::lock_guard<std::recursive_mutex> lock(lock_);
    state_->handlers[type] = std::move(handler);
}

}  // namespace galaxy::coop
