#include "galaxy/net/link.h"

#include <algorithm>
#include <cmath>
#include <cstring>
#include <utility>

namespace galaxy::net {

namespace {

// A frame: its kind, then for data the channel (and 0x80 when more pieces of the message follow), the sequence
// (reliable and ordered) and the order (ordered), then the message's bytes; for acknowledgements their count and the
// sequences.
enum FrameKind : std::uint8_t { kFrameData = 1, kFrameAck = 2 };
constexpr std::uint8_t kMorePieces = 0x80u;
constexpr std::size_t kMaxAcksPerFrame = 255u;
// Sequences this far past the last one in order are refused (a broken or hostile sender).
constexpr std::uint32_t kReceiveWindow = 1u << 16;
// Where the sequence goes in a reliable data frame.
constexpr std::size_t kSequenceAt = 2u;

void put32(std::vector<std::uint8_t>& out, std::uint32_t value) {
    for (int i = 0; i < 4; ++i) out.push_back(static_cast<std::uint8_t>(value >> (8 * i)));
}

std::uint32_t get32(const std::uint8_t* in) {
    return static_cast<std::uint32_t>(in[0]) | static_cast<std::uint32_t>(in[1]) << 8 |
           static_cast<std::uint32_t>(in[2]) << 16 | static_cast<std::uint32_t>(in[3]) << 24;
}

}  // namespace

Link::Link(Send send) : send_(std::move(send)) {}

std::chrono::milliseconds Link::rtt() const noexcept {
    return std::chrono::milliseconds(static_cast<long long>(srtt_ / 1000.0));
}

bool Link::send(Channel channel, std::span<const std::uint8_t> data, Clock::time_point now) {
    if (broken_) return false;
    switch (channel) {
    case Channel::Unreliable: {
        if (data.size() > kMaxSmallMessage) return false;
        std::vector<std::uint8_t> frame{kFrameData, static_cast<std::uint8_t>(Channel::Unreliable)};
        frame.insert(frame.end(), data.begin(), data.end());
        send_(frame);
        return true;
    }
    case Channel::Reliable: {
        if (data.size() > kMaxSmallMessage) return false;
        std::vector<std::uint8_t> frame{kFrameData, static_cast<std::uint8_t>(Channel::Reliable)};
        put32(frame, 0u);
        frame.insert(frame.end(), data.begin(), data.end());
        queue_.push_back(std::move(frame));
        break;
    }
    case Channel::Ordered: {
        if (data.size() > kMaxOrderedMessage) return false;
        std::size_t at = 0u;
        do {
            const std::size_t piece = std::min(kMaxSmallMessage, data.size() - at);
            const bool more = at + piece < data.size();
            std::vector<std::uint8_t> frame{
                kFrameData, static_cast<std::uint8_t>(static_cast<std::uint8_t>(Channel::Ordered) |
                                                      (more ? kMorePieces : 0u))};
            put32(frame, 0u);
            put32(frame, next_order_out_++);
            frame.insert(frame.end(), data.begin() + static_cast<std::ptrdiff_t>(at),
                         data.begin() + static_cast<std::ptrdiff_t>(at + piece));
            queue_.push_back(std::move(frame));
            at += piece;
        } while (at < data.size());
        break;
    }
    default: return false;
    }
    fill_window(now);
    return true;
}

void Link::fill_window(Clock::time_point now) {
    while (!queue_.empty() && in_flight_.size() < kWindow) {
        const std::uint32_t sequence = next_sequence_++;
        Outgoing outgoing;
        outgoing.frame = std::move(queue_.front());
        queue_.pop_front();
        for (int i = 0; i < 4; ++i) {
            outgoing.frame[kSequenceAt + i] = static_cast<std::uint8_t>(sequence >> (8 * i));
        }
        outgoing.first_sent = now;
        transmit(sequence, in_flight_.emplace(sequence, std::move(outgoing)).first->second, now);
    }
}

void Link::transmit(std::uint32_t, Outgoing& outgoing, Clock::time_point now) {
    if (outgoing.attempts > 0u) ++resends_;
    ++outgoing.attempts;
    outgoing.last_sent = now;
    send_(outgoing.frame);
}

Link::Clock::duration Link::timeout(unsigned attempts) const {
    // RFC 6298's timeout, at least 30 ms (a LAN's round trip is far less, a frame of the sender's game is not), then
    // doubled for each try already made, to 2 s.
    const double base = std::max(30000.0, srtt_ + 4.0 * rttvar_);
    const double backed = base * std::pow(2.0, static_cast<double>(std::min(attempts, 7u) - 1u));
    return std::chrono::microseconds(static_cast<long long>(std::min(backed, 2000000.0)));
}

void Link::update(Clock::time_point now) {
    flush_acks();
    if (broken_) return;
    for (auto& [sequence, outgoing] : in_flight_) {
        if (now - outgoing.first_sent > kGiveUp) {
            broken_ = true;
            in_flight_.clear();
            queue_.clear();
            return;
        }
        if (now - outgoing.last_sent >= timeout(outgoing.attempts)) transmit(sequence, outgoing, now);
    }
    fill_window(now);
}

void Link::acknowledge(std::uint32_t sequence, Clock::time_point now) {
    const auto it = in_flight_.find(sequence);
    if (it == in_flight_.end()) return;
    // Karn's rule: only a message sent once times the round trip.
    if (it->second.attempts == 1u) {
        const auto took = std::chrono::duration_cast<std::chrono::microseconds>(now - it->second.first_sent);
        const double sample = static_cast<double>(took.count());
        if (!measured_) {
            srtt_ = sample;
            rttvar_ = sample / 2.0;
            measured_ = true;
        } else {
            rttvar_ = 0.75 * rttvar_ + 0.25 * std::abs(srtt_ - sample);
            srtt_ = 0.875 * srtt_ + 0.125 * sample;
        }
    }
    in_flight_.erase(it);
}

void Link::flush_acks() {
    std::size_t at = 0u;
    while (at < acks_.size()) {
        const std::size_t count = std::min(kMaxAcksPerFrame, acks_.size() - at);
        std::vector<std::uint8_t> frame{kFrameAck, static_cast<std::uint8_t>(count)};
        for (std::size_t i = 0; i < count; ++i) put32(frame, acks_[at + i]);
        send_(frame);
        at += count;
    }
    acks_.clear();
}

void Link::receive(std::span<const std::uint8_t> frame, Clock::time_point now, std::vector<LinkMessage>& out) {
    if (frame.size() < 2u) return;
    if (frame[0] == kFrameAck) {
        const std::size_t count = frame[1];
        if (frame.size() < 2u + 4u * count) return;
        for (std::size_t i = 0; i < count; ++i) acknowledge(get32(frame.data() + 2u + 4u * i), now);
        fill_window(now);
        return;
    }
    if (frame[0] != kFrameData) return;
    const auto channel = static_cast<Channel>(frame[1] & ~kMorePieces);
    const bool more = (frame[1] & kMorePieces) != 0u;
    if (channel == Channel::Unreliable) {
        out.push_back({channel, std::vector<std::uint8_t>(frame.begin() + 2, frame.end())});
        return;
    }
    if (channel != Channel::Reliable && channel != Channel::Ordered) return;
    const std::size_t header = channel == Channel::Ordered ? 10u : 6u;
    if (frame.size() < header) return;
    const std::uint32_t sequence = get32(frame.data() + kSequenceAt);
    if (sequence == 0u || sequence - received_below_ >= kReceiveWindow) {
        // Already received (below the window): acknowledged again, as its acknowledgement was lost. Else refused.
        if (sequence != 0u && sequence < received_below_) acks_.push_back(sequence);
        return;
    }
    acks_.push_back(sequence);
    if (acks_.size() >= kMaxAcksPerFrame) flush_acks();
    if (sequence < received_below_ || received_above_.count(sequence) != 0u) return;  // a repeat
    if (sequence == received_below_) {
        ++received_below_;
        while (!received_above_.empty() && *received_above_.begin() == received_below_) {
            received_above_.erase(received_above_.begin());
            ++received_below_;
        }
    } else {
        received_above_.insert(sequence);
    }
    std::vector<std::uint8_t> data(frame.begin() + static_cast<std::ptrdiff_t>(header), frame.end());
    if (channel == Channel::Reliable) {
        out.push_back({channel, std::move(data)});
        return;
    }
    const std::uint32_t order = get32(frame.data() + 6u);
    if (order - next_order_in_ >= kReceiveWindow) return;
    early_[order] = Piece{more, std::move(data)};
    for (auto it = early_.find(next_order_in_); it != early_.end(); it = early_.find(next_order_in_)) {
        Piece piece = std::move(it->second);
        early_.erase(it);
        ++next_order_in_;
        if (joining_.size() + piece.data.size() > kMaxOrderedMessage) {
            joining_.clear();  // longer than any message sent: dropped
            continue;
        }
        joining_.insert(joining_.end(), piece.data.begin(), piece.data.end());
        if (!piece.more) out.push_back({Channel::Ordered, std::exchange(joining_, {})});
    }
}

}  // namespace galaxy::net
