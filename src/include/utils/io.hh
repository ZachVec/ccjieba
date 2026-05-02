/// @file io.hh
/// @brief Binary I/O streams for compact, portable index serialization.

#pragma once

#include <cstddef>
#include <cstring>
#include <memory>
#include <string>
#include <vector>

namespace ccjieba {

/// @brief Write-only binary output stream backed by a contiguous byte buffer.
///
/// Serializes primitives and strings via `operator<<` chaining. Strings are length-prefixed.
/// No alignment or endianness handling — intended for same-platform persistence.
class bostream {  // NOLINT(readability-identifier-naming)
  std::vector<std::byte> buffer_;

 public:
  bostream() = default;

  auto reserve(size_t n) -> void { buffer_.reserve(n); }

  /// @brief Pointer to the underlying buffer.
  inline auto data() const -> const std::byte * { return buffer_.data(); }

  /// @brief Current buffer size in bytes.
  inline auto size() const -> size_t { return buffer_.size(); }

  /// @brief Append raw bytes to the buffer.
  inline auto dump(const std::byte *data, size_t n) -> bostream & {
    size_t prev = buffer_.size();
    buffer_.resize(prev + n);
    std::memcpy(buffer_.data() + prev, data, n);
    return *this;
  }

  friend auto operator<<(bostream &os, bool value) -> bostream &;
  friend auto operator<<(bostream &os, short value) -> bostream &;
  friend auto operator<<(bostream &os, int value) -> bostream &;
  friend auto operator<<(bostream &os, long value) -> bostream &;
  friend auto operator<<(bostream &os, long long value) -> bostream &;
  friend auto operator<<(bostream &os, unsigned short value) -> bostream &;
  friend auto operator<<(bostream &os, unsigned int value) -> bostream &;
  friend auto operator<<(bostream &os, unsigned long value) -> bostream &;
  friend auto operator<<(bostream &os, unsigned long long value) -> bostream &;
  friend auto operator<<(bostream &os, float value) -> bostream &;
  friend auto operator<<(bostream &os, double value) -> bostream &;
  friend auto operator<<(bostream &os, long double value) -> bostream &;
  friend auto operator<<(bostream &os, char value) -> bostream &;
  friend auto operator<<(bostream &os, signed char value) -> bostream &;
  friend auto operator<<(bostream &os, unsigned char value) -> bostream &;
  friend auto operator<<(bostream &os, char32_t value) -> bostream &;
  friend auto operator<<(bostream &os, const char *value) -> bostream &;

  /// @brief Serialize a string (size-prefixed, followed by raw character data).
  template <typename CharT, typename Traits = std::char_traits<CharT>, typename Alloc = std::allocator<CharT>>
  friend auto operator<<(bostream &os, const std::basic_string<CharT, Traits, Alloc> &value) -> bostream & {
    const auto *data = reinterpret_cast<const std::byte *>(value.data());
    size_t size = value.size();
    return (os << size).dump(data, size * sizeof(CharT));
  }
};

/// @brief Read-only binary input stream with cursor over a byte span.
///
/// Deserializes via `operator>>` chaining. Reads are unchecked after the buffer boundary
/// (the caller should use `operator bool()` to verify). String reads first extract the size,
/// then resize and copy.
class bistream {  // NOLINT(readability-identifier-naming)
  const std::byte *data_;
  const size_t size_;
  size_t cursor_;

 public:
  bistream(const std::byte *data, size_t n) : data_(data), size_(n), cursor_(0) {}

  /// @brief True while the cursor has not advanced past the buffer end.
  inline operator bool() const { return cursor_ <= size_; }

  /// @brief Pointer to current cursor position.
  inline auto data() const -> const std::byte * { return &data_[cursor_]; }

  /// @brief Remaining bytes in the buffer.
  inline auto size() const -> size_t { return size_ - cursor_; }

  /// @brief Advance the cursor by @p n bytes.
  inline auto step(size_t n) -> bistream & {
    cursor_ += n;
    return *this;
  }

  /// @brief Copy @p size bytes from the cursor into @p data, then advance.
  inline auto read(std::byte *data, size_t size) -> bistream & {
    if (size_ >= cursor_ + size) {
      std::memmove(data, this->data(), size);
    }
    return this->step(size);
  }

  friend auto operator>>(bistream &is, bool &value) -> bistream &;
  friend auto operator>>(bistream &is, short &value) -> bistream &;
  friend auto operator>>(bistream &is, int &value) -> bistream &;
  friend auto operator>>(bistream &is, long &value) -> bistream &;
  friend auto operator>>(bistream &is, long long &value) -> bistream &;
  friend auto operator>>(bistream &is, unsigned short &value) -> bistream &;
  friend auto operator>>(bistream &is, unsigned int &value) -> bistream &;
  friend auto operator>>(bistream &is, unsigned long &value) -> bistream &;
  friend auto operator>>(bistream &is, unsigned long long &value) -> bistream &;
  friend auto operator>>(bistream &is, float &value) -> bistream &;
  friend auto operator>>(bistream &is, double &value) -> bistream &;
  friend auto operator>>(bistream &is, long double &value) -> bistream &;
  friend auto operator>>(bistream &is, char &value) -> bistream &;
  friend auto operator>>(bistream &is, signed char &value) -> bistream &;
  friend auto operator>>(bistream &is, unsigned char &value) -> bistream &;
  friend auto operator>>(bistream &is, char32_t &value) -> bistream &;

  /// @brief Deserialize a string: read size, resize, then copy character data.
  template <typename CharT, typename Traits = std::char_traits<CharT>, typename Alloc = std::allocator<CharT>>
  friend auto operator>>(bistream &is, std::basic_string<CharT, Traits, Alloc> &value) -> bistream & {
    size_t size;
    if (not(is >> size)) {
      return is;
    }
    value.resize(size);
    return is.read(reinterpret_cast<std::byte *>(value.data()), size * sizeof(CharT));
  }
};

}  // namespace ccjieba
