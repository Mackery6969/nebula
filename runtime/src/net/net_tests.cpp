// Online co-op's network layer: the channels (galaxy/net/link.h) over a line that loses, repeats and reorders
// datagrams, the direct transport's lobby (galaxy/net/direct_transport.h) and the session over it
// (galaxy/coop_session.h), both over the loopback.
#include "galaxy/coop_session.h"
#include "galaxy/net/bytes.h"
#include "galaxy/net/direct_transport.h"
#include "galaxy/net/link.h"
#include "galaxy/net/udp.h"

#include <algorithm>
#include <chrono>
#include <cstdio>
#include <deque>
#include <functional>
#include <random>
#include <string>
#include <thread>
#include <vector>

namespace {
int s_failures = 0;
void expect(bool ok, const char* what) {
    if (!ok) {
        std::fprintf(stderr, "FAIL: %s\n", what);
        ++s_failures;
    }
}

using namespace std::chrono_literals;
using galaxy::net::Channel;
using galaxy::net::Link;
using galaxy::net::LinkMessage;
using Clock = std::chrono::steady_clock;

std::vector<std::uint8_t> bytes_of(const std::string& text) { return {text.begin(), text.end()}; }
std::string text_of(const std::vector<std::uint8_t>& bytes) { return {bytes.begin(), bytes.end()}; }

// Two links joined by a line that loses `loss` of the datagrams, repeats some and delivers them out of order, on a
// clock the test moves.
struct Line {
    std::mt19937 random{1234u};
    double loss = 0.0;
    std::deque<std::vector<std::uint8_t>> to_b, to_a;
    Link a{[this](std::span<const std::uint8_t> frame) { carry(to_b, frame); }};
    Link b{[this](std::span<const std::uint8_t> frame) { carry(to_a, frame); }};
    Clock::time_point now = Clock::now();
    std::vector<LinkMessage> at_a, at_b;

    void carry(std::deque<std::vector<std::uint8_t>>& queue, std::span<const std::uint8_t> frame) {
        std::uniform_real_distribution<double> chance(0.0, 1.0);
        if (chance(random) < loss) return;
        const std::vector<std::uint8_t> copy(frame.begin(), frame.end());
        // Out of order: sometimes in front of what waits.
        if (!queue.empty() && chance(random) < 0.3) {
            queue.push_front(copy);
        } else {
            queue.push_back(copy);
        }
        if (chance(random) < 0.05) queue.push_back(copy);  // repeated
    }

    void step(std::chrono::milliseconds by = 5ms) {
        now += by;
        while (!to_b.empty()) {
            const std::vector<std::uint8_t> frame = std::move(to_b.front());
            to_b.pop_front();
            b.receive(frame, now, at_b);
        }
        while (!to_a.empty()) {
            const std::vector<std::uint8_t> frame = std::move(to_a.front());
            to_a.pop_front();
            a.receive(frame, now, at_a);
        }
        a.update(now);
        b.update(now);
    }
};

void test_link_perfect_line() {
    Line line;
    expect(line.a.send(Channel::Unreliable, bytes_of("snap"), line.now), "an unreliable message is sent");
    expect(line.a.send(Channel::Reliable, bytes_of("once"), line.now), "a reliable message is sent");
    expect(line.a.send(Channel::Ordered, bytes_of("first"), line.now), "an ordered message is sent");
    for (int i = 0; i < 10; ++i) line.step();
    expect(line.at_b.size() == 3u, "the three arrive over a perfect line");
    expect(line.a.in_flight() == 0u, "and every reliable one is acknowledged");
    expect(line.a.resends() == 0u, "with nothing sent twice");
    expect(!line.a.send(Channel::Unreliable, std::vector<std::uint8_t>(galaxy::net::kMaxSmallMessage + 1u), line.now),
           "an unreliable message longer than a datagram is refused");
}

void test_link_lossy_line() {
    Line line;
    line.loss = 0.3;
    constexpr int kCount = 400;
    for (int i = 0; i < kCount; ++i) {
        expect(line.a.send(Channel::Ordered, bytes_of("ordered " + std::to_string(i)), line.now), "ordered queued");
        expect(line.a.send(Channel::Reliable, bytes_of("reliable " + std::to_string(i)), line.now), "reliable queued");
        if (i % 4 == 0) line.step(1ms);
    }
    for (int i = 0; i < 2000 && (line.a.in_flight() != 0u || line.a.queued() != 0u); ++i) line.step(10ms);
    std::vector<std::string> ordered;
    std::vector<std::string> reliable;
    for (const LinkMessage& message : line.at_b) {
        (message.channel == Channel::Ordered ? ordered : reliable).push_back(text_of(message.data));
    }
    expect(!line.a.broken(), "a lossy line doesn't break the link");
    expect(ordered.size() == static_cast<std::size_t>(kCount), "every ordered message arrives once");
    bool in_order = ordered.size() == static_cast<std::size_t>(kCount);
    for (std::size_t i = 0; in_order && i < ordered.size(); ++i) in_order = ordered[i] == "ordered " + std::to_string(i);
    expect(in_order, "in the order sent");
    std::sort(reliable.begin(), reliable.end());
    expect(std::adjacent_find(reliable.begin(), reliable.end()) == reliable.end() &&
               reliable.size() == static_cast<std::size_t>(kCount),
           "every reliable message arrives once");
    expect(line.a.resends() > 0u, "lost ones were sent again");
}

void test_link_long_message() {
    Line line;
    line.loss = 0.1;
    std::vector<std::uint8_t> save(300000u);
    for (std::size_t i = 0; i < save.size(); ++i) save[i] = static_cast<std::uint8_t>(i * 7u + (i >> 9));
    expect(line.a.send(Channel::Ordered, bytes_of("before"), line.now), "a message before");
    expect(line.a.send(Channel::Ordered, save, line.now), "a long ordered message is cut up");
    expect(line.a.send(Channel::Ordered, bytes_of("after"), line.now), "a message after");
    expect(line.a.queued() > 0u, "more pieces than the window holds wait");
    for (int i = 0; i < 4000 && (line.a.in_flight() != 0u || line.a.queued() != 0u); ++i) line.step(10ms);
    expect(line.at_b.size() == 3u, "it arrives as one message among the others");
    expect(line.at_b.size() == 3u && line.at_b[1].data == save, "joined again as sent");
    expect(line.at_b.size() == 3u && text_of(line.at_b[0].data) == "before" && text_of(line.at_b[2].data) == "after",
           "in its place");
}

void test_link_gives_up() {
    Line line;
    line.loss = 1.0;
    expect(line.a.send(Channel::Reliable, bytes_of("lost"), line.now), "a message into nothing");
    for (int i = 0; i < 200 && !line.a.broken(); ++i) line.step(100ms);
    expect(line.a.broken(), "the link breaks once it went unacknowledged long enough");
    expect(!line.a.send(Channel::Reliable, bytes_of("more"), line.now), "and sends nothing more");
}

void test_bytes() {
    galaxy::net::ByteWriter out;
    out.u8(7).u16(0xbeef).u32(0xdeadbeef).u64(0x0123456789abcdefull).f32(1.5f).str("name").blob(bytes_of("blob"));
    galaxy::net::ByteReader in(out.bytes());
    expect(in.u8() == 7 && in.u16() == 0xbeef && in.u32() == 0xdeadbeef && in.u64() == 0x0123456789abcdefull &&
               in.f32() == 1.5f && in.str() == "name" && text_of(in.blob(16)) == "blob" && in.ok() && in.left() == 0u,
           "numbers, strings and bytes read back as written");
    (void)in.u32();
    expect(!in.ok(), "reading past the end is noticed");
}

void test_addresses() {
    galaxy::net::Address four;
    galaxy::net::Address six;
    expect(galaxy::net::resolve("127.0.0.1", 7777u, four) && four.to_string() == "127.0.0.1:7777" && !four.ipv6(),
           "an IPv4 address resolves");
    if (galaxy::net::resolve("::1", 7778u, six)) {
        expect(six.ipv6() && six.to_string() == "[::1]:7778" && six.port() == 7778u, "an IPv6 address resolves");
    }
    expect(!(four == six), "different addresses differ");
}

// Updates the transports until `done` or two seconds pass.
// Callbacks by name, the others left empty.
galaxy::net::TransportCallbacks callbacks(std::function<void(galaxy::net::TransportCallbacks&)> fill) {
    galaxy::net::TransportCallbacks out;
    fill(out);
    return out;
}

galaxy::net::TransportCallbacks on_error(std::function<void(galaxy::net::NetError)> handle) {
    galaxy::net::TransportCallbacks out;
    out.on_error = [handle](galaxy::net::NetError error, galaxy::net::PeerId) { handle(error); };
    return out;
}

bool pump(std::vector<galaxy::net::DirectTransport*> transports, const std::function<bool()>& done,
          std::chrono::milliseconds limit = 3000ms) {
    const Clock::time_point until = Clock::now() + limit;
    while (Clock::now() < until) {
        for (auto* transport : transports) transport->update(1ms);
        if (done()) return true;
    }
    return false;
}

void test_transport_lobby() {
    using namespace galaxy::net;
    DirectTransport host, a, b;
    std::vector<std::string> host_got, a_got, b_got;
    PeerId a_id = kNoPeer, b_id = kNoPeer;
    int a_joined_peers = 0;
    bool b_left_seen_by_a = false;
    NetError a_error = NetError::None;
    host.set_callbacks(callbacks([&](TransportCallbacks& c) {
        c.on_receive = [&](PeerId, Channel, std::span<const std::uint8_t> data) {
            host_got.emplace_back(data.begin(), data.end());
        };
    }));
    a.set_callbacks(callbacks([&](TransportCallbacks& c) {
        c.on_lobby_joined = [&](PeerId self, PeerId, std::uint64_t) { a_id = self; };
        c.on_peer_joined = [&](PeerId) { ++a_joined_peers; };
        c.on_peer_left = [&](PeerId, bool intentional) { b_left_seen_by_a = intentional; };
        c.on_receive = [&](PeerId, Channel, std::span<const std::uint8_t> data) {
            a_got.emplace_back(data.begin(), data.end());
        };
        c.on_error = [&](NetError error, PeerId) { a_error = error; };
    }));
    b.set_callbacks(callbacks([&](TransportCallbacks& c) {
        c.on_lobby_joined = [&](PeerId self, PeerId, std::uint64_t) { b_id = self; };
        c.on_receive = [&](PeerId, Channel, std::span<const std::uint8_t> data) {
            b_got.emplace_back(data.begin(), data.end());
        };
    }));

    LobbyConfig config;
    config.password = "secret";
    config.max_members = 3u;
    expect(host.lobby_create(config) && host.in_lobby() && host.is_host() && host.port() != 0u, "the host opens a lobby");
    const std::uint16_t port = host.port();

    // A wrong password.
    DirectTransport wrong;
    NetError wrong_error = NetError::None;
    wrong.set_callbacks(on_error([&](NetError error) { wrong_error = error; }));
    expect(wrong.lobby_join({"127.0.0.1", port, "nope"}), "a join with the wrong password starts");
    expect(pump({&host, &wrong}, [&] { return wrong_error != NetError::None; }) &&
               wrong_error == NetError::PasswordIncorrect && !wrong.in_lobby(),
           "and is refused for its password");

    host.set_test_loss(0.2);
    a.set_test_loss(0.2);
    expect(a.lobby_join({"127.0.0.1", port, "secret"}), "a member joins");
    expect(pump({&host, &a}, [&] { return a.in_lobby(); }), "and is let in over a lossy line");
    expect(a.self() == a_id && a.owner() == host.self() && a_id != host.self(), "with its own id and the host's");
    expect(b.lobby_join({"localhost", port, "secret"}), "another member joins");
    expect(pump({&host, &a, &b}, [&] { return b.in_lobby() && a_joined_peers == 1; }),
           "and the first member hears of it");

    // Full: three members the most.
    DirectTransport full;
    NetError full_error = NetError::None;
    full.set_callbacks(on_error([&](NetError error) { full_error = error; }));
    (void)full.lobby_join({"127.0.0.1", port, "secret"});
    expect(pump({&host, &a, &b, &full}, [&] { return full_error != NetError::None; }) &&
               full_error == NetError::LobbyFull,
           "a member past the lobby's size is refused");

    // Messages: to the host, to everyone (the host passes it on), to one member.
    for (int i = 0; i < 50; ++i) {
        expect(a.send_to(kEveryone, Channel::Ordered, bytes_of("hello " + std::to_string(i))), "a member sends");
    }
    expect(host.send_to(b_id, Channel::Reliable, bytes_of("just b")), "the host sends to one member");
    expect(a.send_to(b_id, Channel::Ordered, bytes_of("a to b")), "a member sends to another");
    expect(pump({&host, &a, &b}, [&] { return host_got.size() == 50u && b_got.size() == 52u; }),
           "every message reaches whom it is for");
    bool in_order = b_got.size() == 52u;
    std::size_t hello = 0u;
    for (const std::string& got : b_got) {
        if (got.rfind("hello ", 0) == 0) in_order = in_order && got == "hello " + std::to_string(hello++);
    }
    expect(in_order, "a member's ordered messages reach another in order, through the host");
    expect(a_got.empty(), "and not back to their sender");

    // Leaving: the others hear of it.
    b.lobby_leave();
    expect(pump({&host, &a}, [&] { return b_left_seen_by_a; }), "a member leaving is told to the others");
    host.lobby_leave();
    expect(pump({&a}, [&] { return a_error != NetError::None; }) && a_error == NetError::HostLeft && !a.in_lobby(),
           "the host closing the lobby ends it for its members");

    // Nobody there.
    DirectTransport alone;
    NetError alone_error = NetError::None;
    alone.set_callbacks(on_error([&](NetError error) { alone_error = error; }));
    (void)alone.lobby_join({"127.0.0.1", port, ""});
    expect(pump({&alone}, [&] { return alone_error != NetError::None; }, 12000ms) &&
               alone_error == NetError::LobbyNotFound,
           "a join nobody answers fails");
}

// Until `done` or the time passes, the session's thread doing the work.
bool wait_for(const std::function<bool()>& done, std::chrono::milliseconds limit = 5000ms) {
    const Clock::time_point until = Clock::now() + limit;
    while (Clock::now() < until) {
        if (done()) return true;
        std::this_thread::sleep_for(5ms);
    }
    return done();
}

bool has_event(const std::vector<galaxy::coop::SessionEvent>& events, galaxy::coop::SessionEvent::Kind kind) {
    return std::any_of(events.begin(), events.end(), [kind](const auto& e) { return e.kind == kind; });
}

void test_session() {
    using namespace galaxy::coop;
    using Kind = SessionEvent::Kind;
    Session host_session;
    HostConfig host;
    host.port = 0u;
    host.room.max_players = 3u;
    host.room.cheats = true;
    host.room.password = "pw";
    host.game_version = "test 1";
    host.name = "HOSTY";
    host.save = [] {
        RoomSave save;
        save.file = 3u;
        save.files.push_back({"GameData.bin", std::vector<std::uint8_t>(20000u, 0x5au)});
        return save;
    };
    expect(host_session.host(host) && host_session.is_host(), "a session hosts");
    const std::uint16_t port = host_session.port();
    expect(port != 0u, "on a port");
    expect(host_session.players().size() == 1u && host_session.players()[0].host &&
               host_session.players()[0].name == "HOSTY",
           "with the host as its only player");

    // Another version is refused.
    Session old;
    expect(old.join({"127.0.0.1", port, "pw", "test 0", "OLD", galaxy::CoopCharacter::Mario}), "an old game joins");
    std::vector<SessionEvent> old_events;
    expect(wait_for([&] {
               for (auto& e : old.take_events()) old_events.push_back(e);
               return has_event(old_events, Kind::JoinFailed);
           }),
           "and is refused");
    expect(!old_events.empty() && old_events.back().text.find("ANOTHER VERSION") != std::string::npos,
           "for its version");

    Session joiner;
    expect(joiner.join({"127.0.0.1", port, "pw", "test 1", "LUIGI FAN", galaxy::CoopCharacter::Luigi}), "a player joins");
    std::vector<SessionEvent> joiner_events;
    expect(wait_for([&] {
               for (auto& e : joiner.take_events()) joiner_events.push_back(e);
               return has_event(joiner_events, Kind::Joined);
           }),
           "and is let in");
    const std::optional<RoomSave> save = joiner.take_room_save();
    expect(save && save->file == 3u && save->files.size() == 1u && save->files[0].name == "GameData.bin" &&
               save->files[0].bytes == std::vector<std::uint8_t>(20000u, 0x5au),
           "with the host's save");
    expect(joiner.room().cheats && joiner.room().max_players == 3u, "and the room's settings");
    const std::vector<SessionPlayer> players = joiner.players();
    expect(players.size() == 2u && players[0].host && players[0].name == "HOSTY" && players[1].index == 1u &&
               players[1].local && players[1].character == galaxy::CoopCharacter::Luigi,
           "and who is playing");
    std::vector<SessionEvent> host_events = host_session.take_events();
    expect(has_event(host_events, Kind::PlayerJoined) && host_session.players().size() == 2u,
           "the host sees the player join");

    // A feature's messages, both ways and passed on.
    std::atomic<int> host_got{0}, joiner_got{0}, third_got{0};
    host_session.on_message(kFirstFeatureMessage, [&](const SessionPlayer& from, std::span<const std::uint8_t> data) {
        if (from.index == 1u && text_of({data.begin(), data.end()}) == "hi") ++host_got;
    });
    joiner.on_message(kFirstFeatureMessage, [&](const SessionPlayer& from, std::span<const std::uint8_t>) {
        if (from.host) ++joiner_got;
    });
    Session third;
    third.on_message(kFirstFeatureMessage, [&](const SessionPlayer& from, std::span<const std::uint8_t>) {
        if (from.index == 1u) ++third_got;
    });
    expect(third.join({"127.0.0.1", port, "pw", "test 1", "THIRD", galaxy::CoopCharacter::Mario}), "a third joins");
    expect(wait_for([&] { return third.state() == SessionState::Joined && joiner.players().size() == 3u; }),
           "and everyone knows of it");
    expect(joiner.send(kFirstFeatureMessage, galaxy::net::Channel::Reliable, bytes_of("hi")), "a player sends");
    expect(host_session.send(kFirstFeatureMessage, galaxy::net::Channel::Unreliable, bytes_of("yo")), "the host sends");
    expect(wait_for([&] { return host_got == 1 && joiner_got >= 1 && third_got == 1; }),
           "a feature's messages reach every player");

    // Full: the room holds three.
    Session fourth;
    (void)fourth.join({"127.0.0.1", port, "pw", "test 1", "FOURTH", galaxy::CoopCharacter::Mario});
    std::vector<SessionEvent> fourth_events;
    expect(wait_for([&] {
               for (auto& e : fourth.take_events()) fourth_events.push_back(e);
               return has_event(fourth_events, Kind::JoinFailed);
           }),
           "a player past the room's size is refused");

    // The host changes the room.
    galaxy::RuntimeCoopSettings room = host_session.room();
    room.cheats = false;
    host_session.set_room(room);
    expect(wait_for([&] { return !joiner.room().cheats; }), "the host's room changes reach the players");

    third.leave();
    expect(wait_for([&] { return host_session.players().size() == 2u && joiner.players().size() == 2u; }),
           "a player leaving leaves everyone's list");
    host_session.leave();
    expect(wait_for([&] {
               for (auto& e : joiner.take_events()) joiner_events.push_back(e);
               return has_event(joiner_events, Kind::Closed);
           }),
           "the host leaving closes the room for the others");
    expect(joiner.state() == SessionState::Idle, "who are back to no session");
}

}  // namespace

int main() {
    test_bytes();
    test_link_perfect_line();
    test_link_lossy_line();
    test_link_long_message();
    test_link_gives_up();
    test_addresses();
    test_transport_lobby();
    test_session();
    if (s_failures == 0) std::printf("net tests passed\n");
    return s_failures == 0 ? 0 : 1;
}
