/// @file str_utils.hh
/// @brief String utilities: split, strip, and UTF-8/UTF-32 codec.

#pragma once

#include <cstddef>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

namespace ccjieba {

/// @brief Split a string_view on delimiter characters.
/// @tparam CharT Character type.
/// @tparam Traits Character traits.
/// @tparam Alloc Allocator for the result vector.
/// @param str The string to split.
/// @param chars Delimiter characters (any one causes a split).
/// @return Vector of substrings (views into @p str).
template <typename CharT, typename Traits = std::char_traits<CharT>,
          typename Alloc = std::allocator<std::basic_string_view<CharT, Traits>>>
auto split(std::basic_string_view<CharT, Traits> str, std::basic_string_view<CharT, Traits> chars)
    -> std::vector<std::basic_string_view<CharT, Traits>, Alloc> {
  size_t i = 0;
  std::vector<std::basic_string_view<CharT, Traits>, Alloc> ret;
  size_t j = str.find_first_of(chars, 0);
  for (; j != std::basic_string_view<CharT, Traits>::npos;) {
    ret.emplace_back(str.substr(i, j - i));
    i = j + 1;
    j = str.find_first_of(chars, i);
  }
  ret.emplace_back(str.substr(i));
  return ret;
}

/// @brief Strip leading and trailing characters from a string_view.
/// @tparam CharT Character type.
/// @tparam Traits Character traits.
/// @tparam Alloc Allocator (unused, present for compatibility with split).
/// @param str The string to strip.
/// @param chars Characters to remove.
/// @return A substring view with leading/trailing @p chars removed.
template <typename CharT, typename Traits = std::char_traits<CharT>,
          typename Alloc = std::allocator<std::basic_string_view<CharT, Traits>>>
auto strip(std::basic_string_view<CharT, Traits> str, std::basic_string_view<CharT, Traits> chars)
    -> std::basic_string_view<CharT, Traits> {
  size_t i = str.find_first_not_of(chars);
  if (i == std::basic_string_view<CharT, Traits>::npos) {
    return str;
  }
  size_t j = str.find_last_not_of(chars);
  return str.substr(i, j - i + 1);
}

/// @brief Decode a UTF-8 string to UTF-32.
/// @param str Input UTF-8 string_view.
/// @param[out] ret Output UTF-32 string (cleared, then appended).
/// @return false if invalid UTF-8 is encountered.
auto utf8_to_utf32(std::string_view str, std::u32string &ret) -> bool;

/// @brief Encode a UTF-32 string to UTF-8.
/// @param str Input UTF-32 string_view.
/// @param[out] ret Output UTF-8 string (cleared, then appended).
/// @return Always true (validates only 4-byte code point bounds).
auto utf32_to_utf8(std::u32string_view str, std::string &ret) -> bool;

}  // namespace ccjieba
