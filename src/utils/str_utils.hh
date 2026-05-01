#pragma once

#include <cstddef>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

namespace ccjieba {

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

auto utf8_to_utf32(std::string_view str, std::u32string &ret) -> bool;

auto utf32_to_utf8(std::u32string_view str, std::string &ret) -> bool;

}  // namespace ccjieba
