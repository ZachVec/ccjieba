/// @file str_utils.cc
/// @brief UTF-8/UTF-32 codec implementation: manual encoding without libiconv.

#include "utils/str_utils.hh"

#include <cstddef>
#include <string>
#include <string_view>

namespace ccjieba {

/// @brief Decode UTF-8 to UTF-32.
///
/// Handles 1–4 byte sequences. Returns false on invalid lead bytes,
/// truncated sequences, or missing continuation bytes (which must match 10xxxxxx).
auto utf8_to_utf32(std::string_view str, std::u32string &ret) -> bool {
  size_t size = str.size();
  ret.clear();
  ret.reserve(size);
  for (size_t i = 0, length; i < size; i += length) {
    if (not(str[i] & 0x80)) {
      length = 1;
    } else {
      switch (str[i] & 0b1111'0000) {
        case 0b1100'0000:
          length = 2;
          break;
        case 0b1110'0000:
          length = 3;
          break;
        case 0b1111'0000:
          length = 4;
          break;
        default:
          return false;
      }
    }
    if (i + length > size) {
      return false;
    }
    char32_t c = ((1U << (8 - length)) - 1) & str[i];
    for (size_t j = 1; j < length; ++j) {
      if ((str[i + j] & 0x80) != 0x80) {
        return false;
      }
      c <<= 6;
      c |= (0b0011'1111 & str[i + j]);
    }
    ret.push_back(c);
  }
  ret.shrink_to_fit();
  return true;
}

/// @brief Encode UTF-32 to UTF-8.
///
/// Produces 1–4 byte sequences based on code point range.
/// Code points above U+10FFFF are silently ignored (not appended).
auto utf32_to_utf8(std::u32string_view str, std::string &ret) -> bool {
  size_t size = str.size();
  ret.clear();
  ret.reserve(4 * size);
  for (const char32_t &c : str) {
    if (c <= 0x7F) {
      ret.push_back(static_cast<char>(c));
    } else if (c <= 0x7FF) {
      ret.push_back(static_cast<char>(0xC0 | ((c >> 6) & 0x1F)));
      ret.push_back(static_cast<char>(0x80 | (c & 0x3F)));
    } else if (c <= 0xFFFF) {
      ret.push_back(static_cast<char>(0xE0 | ((c >> 12) & 0x0F)));
      ret.push_back(static_cast<char>(0x80 | ((c >> 6) & 0x3F)));
      ret.push_back(static_cast<char>(0x80 | (c & 0x3F)));
    } else if (c <= 0x10FFFF) {
      ret.push_back(static_cast<char>(0xF0 | ((c >> 18) & 0x07)));
      ret.push_back(static_cast<char>(0x80 | ((c >> 12) & 0x3F)));
      ret.push_back(static_cast<char>(0x80 | ((c >> 6) & 0x3F)));
      ret.push_back(static_cast<char>(0x80 | (c & 0x3F)));
    }
  }
  ret.shrink_to_fit();
  return true;
}

}  // namespace ccjieba
