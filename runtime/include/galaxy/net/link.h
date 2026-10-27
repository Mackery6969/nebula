// Online co-op's channels (docs/COOP_PLAN.md, "Phase 1"): what one end of a connection keeps to send messages over
// UDP, which loses, repeats and reorders datagrams. In the spirit of sm64coopdx's packet_reliable.c and
// packet_ordered.c, written for Nebula:
//
// - Unreliable: sent once, maybe lost or out of order (player snapshots: the next one replaces it).
// - Reliable: sent again until the other end acknowledges it, delivered once, in any order.
// - Ordered: reliable, and delivered in the order sent; a long one (a save, a mod) is cut into datagrams and joined
//   again.
//
// The link knows no sockets: it hands the datagrams it wants sent to a function and takes the ones received, so the
// tests can lose and reorder them at will. Not thread-safe: one thread drives it.
#pragma once

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <deque>
#include <functional>
#include <map>
#include <set>
#include <span>
#include <vector>

namespace galaxy::net {

enum class Channel : std::uint8_t { Unreliable = 0, Reliable = 1, Ordered = 2 };

// The largest datagram Nebula sends: under the internet's usual MTU (1280 for IPv6) with its headers.
inline constexpr std::size_t kMaxDatagram = 1200u;
// The largest message: an unreliable or reliable one fits one datagram; an ordered one is cut up, to 64 MiB.
inline constexpr std::size_t kMaxSmallMessage = 1100u;
inline constexpr std::size_t kMaxOrderedMessage = 64u << 20;

struct LinkMessage {
    Channel channel = Channel::Unreliable;
    std::vector<std::uint8_t> data;
};

class Link {
public:
    using Clock = std::chrono::steady_clock;
    // A datagram to send: the link's frame, which the transport wraps in its own header.
    using Send = std::function<void(std::span<const std::uint8_t>)>;

    explicit Link(Send send);

    // Queues a message. False when it is too long for its channel or the link is broken.
    bool send(Channel channel, std::span<const std::uint8_t> data, Clock::time_point now);
    // A frame from the other end: the messages it completes, in the order to act on them, appended to `out`.
    // Malformed frames are ignored.
    void receive(std::span<const std::uint8_t> frame, Clock::time_point now, std::vector<LinkMessage>& out);
    // Sends what is due: acknowledgements, messages waiting for room and the ones not acknowledged in time.
    void update(Clock::time_point now);

    // A reliable message went unacknowledged too long: the other end is gone.
    [[nodiscard]] bool broken() const noexcept { return broken_; }
    // The round trip, smoothed (100 ms until measured).
    [[nodiscard]] std::chrono::milliseconds rtt() const noexcept;
    // Reliable messages sent and not yet acknowledged, and waiting to be sent.
    [[nodiscard]] std::size_t in_flight() const noexcept { return in_flight_.size(); }
    [[nodiscard]] std::size_t queued() const noexcept { return queue_.size(); }
    [[nodiscard]] std::uint64_t resends() const noexcept { return resends_; }

    // How long a reliable message is sent again before the link gives up.
    static constexpr std::chrono::seconds kGiveUp{15};
    // Reliable datagrams in flight at once; the rest wait (a download's flow control).
    static constexpr std::size_t kWindow = 256u;

private:
    struct Outgoing {
        std::vector<std::uint8_t> frame;
        Clock::time_point first_sent{};
        Clock::time_point last_sent{};
        unsigned attempts = 0u;
    };
    struct Piece {
        bool more = false;
        std::vector<std::uint8_t> data;
    };

    void transmit(std::uint32_t sequence, Outgoing& outgoing, Clock::time_point now);
    void fill_window(Clock::time_point now);
    void acknowledge(std::uint32_t sequence, Clock::time_point now);
    void flush_acks();
    [[nodiscard]] Clock::duration timeout(unsigned attempts) const;

    Send send_;
    bool broken_ = false;
    std::uint32_t next_sequence_ = 1u;
    std::uint32_t next_order_out_ = 1u;
    std::deque<std::vector<std::uint8_t>> queue_;  // frames waiting for room in the window, sequence still to fill
    std::map<std::uint32_t, Outgoing> in_flight_;
    // Received: every sequence below `received_below_`, and the ones in `received_above_`.
    std::uint32_t received_below_ = 1u;
    std::set<std::uint32_t> received_above_;
    std::vector<std::uint32_t> acks_;
    // Ordered: the next to deliver, the ones that came early, and a message being joined from its pieces.
    std::uint32_t next_order_in_ = 1u;
    std::map<std::uint32_t, Piece> early_;
    std::vector<std::uint8_t> joining_;
    // Round trip estimate (RFC 6298), in microseconds.
    double srtt_ = 100000.0;
    double rttvar_ = 50000.0;
    bool measured_ = false;
    std::uint64_t resends_ = 0u;
};

}  // namespace galaxy::net
