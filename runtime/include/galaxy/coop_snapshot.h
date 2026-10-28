// Online co-op's player snapshots (docs/COOP_PLAN.md "Remote players"): what each player sends of its Mario 30 times a
// second, unreliably, and how the others draw it. A snapshot is the instance the player is in (the stage and the
// scenario), where it is and how it is turned, and the animation playing. RemotePlayers keeps the latest few of each
// other player and gives them back a little in the past, interpolated between two, so a lost or late datagram doesn't
// show as a jump.
#pragma once

#include "galaxy/net/transport.h"
#include "galaxy/runtime_settings.h"

#include <array>
#include <chrono>
#include <cstdint>
#include <deque>
#include <map>
#include <mutex>
#include <optional>
#include <span>
#include <string>
#include <vector>

namespace galaxy::coop {

// The session's message type the snapshots go as (galaxy/coop_session.h: the features' first).
inline constexpr std::uint8_t kSnapshotMessage = 32u;
// How often a player sends its own.
inline constexpr std::chrono::milliseconds kSnapshotInterval{33};
// How far behind the latest snapshot the others are drawn: a little over two snapshots, so one may be lost.
inline constexpr std::chrono::milliseconds kSnapshotDelay{100};
// A player whose last snapshot is older than this isn't drawn (they're loading, or their datagrams stopped).
inline constexpr std::chrono::milliseconds kSnapshotStale{1000};

struct PlayerSnapshot {
    std::uint32_t sequence = 0u;  // counted up by the sender: an older one than the last is dropped
    std::string stage;
    int scenario = 0;
    std::array<float, 3> position{};
    std::array<float, 3> rotation{};  // degrees
    std::array<float, 3> front{};     // the model's facing and its up (its base matrix's Z and Y), unit length
    std::array<float, 3> up{};
    std::array<float, 3> gravity{};
    std::array<float, 3> velocity{};
    std::string group;      // the animation (XanimePlayer::getCurrentAnimationName)
    std::string animation;  // the bck it plays (getCurrentBckName)
    float frame = 0.0f;
    float rate = 1.0f;
    std::array<float, 4> weights{};  // the animation's tracks' weights (XanimePlayer::mWeights)
    std::uint32_t health = 0u;
    std::uint32_t mode = 0u;
    std::uint32_t flags = 0u;  // NEBULA_COOP_PLAYER_*
};

// Longest names kept from a snapshot.
inline constexpr std::size_t kSnapshotStageLimit = 47u;
inline constexpr std::size_t kSnapshotAnimationLimit = 47u;

[[nodiscard]] std::vector<std::uint8_t> encode_snapshot(const PlayerSnapshot& snapshot);
// Nothing when it's cut short or not a snapshot.
[[nodiscard]] std::optional<PlayerSnapshot> decode_snapshot(std::span<const std::uint8_t> data);

// Between two snapshots of the same player: positions and directions along the way, angles the short way round, the
// animation the later one's (its frame moved along when both play the same).
[[nodiscard]] PlayerSnapshot blend_snapshots(const PlayerSnapshot& from, const PlayerSnapshot& to, float t);

struct RemotePlayer {
    net::PeerId id = net::kNoPeer;
    unsigned index = 0u;  // the session's number: 0 the host
    std::string name;
    CoopCharacter character = CoopCharacter::Mario;
    PlayerSnapshot snapshot;
};

// The other players' snapshots as they come (on the session's thread), and as the game draws them (on its own).
class RemotePlayers {
public:
    using Clock = std::chrono::steady_clock;

    // A snapshot from a player, received at `now`.
    void receive(net::PeerId id, unsigned index, const std::string& name, CoopCharacter character,
                 std::span<const std::uint8_t> data, Clock::time_point now = Clock::now());
    void remove(net::PeerId id);
    void clear();
    // Everyone with a fresh snapshot in the given instance, as at `now` less kSnapshotDelay, by the session's number.
    [[nodiscard]] std::vector<RemotePlayer> sample(const std::string& stage, int scenario,
                                                   Clock::time_point now = Clock::now()) const;

private:
    struct Received {
        Clock::time_point at;
        PlayerSnapshot snapshot;
    };
    struct Player {
        unsigned index = 0u;
        std::string name;
        CoopCharacter character = CoopCharacter::Mario;
        std::deque<Received> received;  // oldest first
    };

    mutable std::mutex lock_;
    std::map<net::PeerId, Player> players_;
};

}  // namespace galaxy::coop
