// Online co-op's messages as bytes: little-endian numbers and length-prefixed strings, written and read back. A
// reader past its end reads zeros and stays !ok(), so a malformed message is noticed once, after reading it all.
#pragma once

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <span>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace galaxy::net {

class ByteWriter {
public:
    ByteWriter& u8(std::uint8_t value) {
        bytes_.push_back(value);
        return *this;
    }
    ByteWriter& u16(std::uint16_t value) { return number(value, 2); }
    ByteWriter& u32(std::uint32_t value) { return number(value, 4); }
    ByteWriter& u64(std::uint64_t value) { return number(value, 8); }
    ByteWriter& f32(float value) {
        std::uint32_t bits = 0u;
        std::memcpy(&bits, &value, sizeof(bits));
        return u32(bits);
    }
    // A string of at most 65535 bytes (longer is cut).
    ByteWriter& str(std::string_view text) {
        const std::size_t length = text.size() < 0xffffu ? text.size() : 0xffffu;
        u16(static_cast<std::uint16_t>(length));
        bytes_.insert(bytes_.end(), text.begin(), text.begin() + static_cast<std::ptrdiff_t>(length));
        return *this;
    }
    // Bytes with their length first (32 bits).
    ByteWriter& blob(std::span<const std::uint8_t> data) {
        u32(static_cast<std::uint32_t>(data.size()));
        return raw(data);
    }
    ByteWriter& raw(std::span<const std::uint8_t> data) {
        bytes_.insert(bytes_.end(), data.begin(), data.end());
        return *this;
    }

    [[nodiscard]] const std::vector<std::uint8_t>& bytes() const noexcept { return bytes_; }
    [[nodiscard]] std::vector<std::uint8_t> take() noexcept { return std::move(bytes_); }

private:
    ByteWriter& number(std::uint64_t value, int size) {
        for (int i = 0; i < size; ++i) bytes_.push_back(static_cast<std::uint8_t>(value >> (8 * i)));
        return *this;
    }

    std::vector<std::uint8_t> bytes_;
};

class ByteReader {
public:
    explicit ByteReader(std::span<const std::uint8_t> data) : data_(data) {}

    std::uint8_t u8() { return static_cast<std::uint8_t>(number(1)); }
    std::uint16_t u16() { return static_cast<std::uint16_t>(number(2)); }
    std::uint32_t u32() { return static_cast<std::uint32_t>(number(4)); }
    std::uint64_t u64() { return number(8); }
    float f32() {
        const std::uint32_t bits = u32();
        float value = 0.0f;
        std::memcpy(&value, &bits, sizeof(value));
        return value;
    }
    // A string, at most `limit` bytes kept (the rest read past).
    std::string str(std::size_t limit = 0xffffu) {
        const std::size_t length = u16();
        if (!take(length)) return {};
        const std::size_t kept = length < limit ? length : limit;
        return std::string(reinterpret_cast<const char*>(data_.data() + at_ - length), kept);
    }
    std::vector<std::uint8_t> blob(std::size_t limit) {
        const std::size_t length = u32();
        if (length > limit || !take(length)) {
            ok_ = false;
            return {};
        }
        return std::vector<std::uint8_t>(data_.begin() + static_cast<std::ptrdiff_t>(at_ - length),
                                         data_.begin() + static_cast<std::ptrdiff_t>(at_));
    }
    // What is left, all of it.
    std::span<const std::uint8_t> rest() {
        const std::span<const std::uint8_t> out = data_.subspan(at_);
        at_ = data_.size();
        return out;
    }

    [[nodiscard]] bool ok() const noexcept { return ok_; }
    [[nodiscard]] std::size_t left() const noexcept { return data_.size() - at_; }

private:
    bool take(std::size_t size) {
        if (!ok_ || data_.size() - at_ < size) {
            ok_ = false;
            return false;
        }
        at_ += size;
        return true;
    }
    std::uint64_t number(int size) {
        if (!take(static_cast<std::size_t>(size))) return 0u;
        std::uint64_t value = 0u;
        const std::size_t first = at_ - static_cast<std::size_t>(size);
        for (int i = 0; i < size; ++i) {
            value |= static_cast<std::uint64_t>(data_[first + static_cast<std::size_t>(i)]) << (8 * i);
        }
        return value;
    }

    std::span<const std::uint8_t> data_;
    std::size_t at_ = 0u;
    bool ok_ = true;
};

}  // namespace galaxy::net
