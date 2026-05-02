/// @file stop_words.hh
/// @brief Stop-words dictionary for filtering during keyword extraction.

#pragma once

#include <functional>
#include <istream>
#include <string>
#include <string_view>
#include <unordered_set>

#include "utils/io.hh"

namespace ccjieba {

/// @brief Set of stop-words filtered out during keyword extraction.
///
/// Uses heterogeneous hash/equality for `string_view` membership test without allocation.
class StopWordsDictionary {
  std::unordered_set<std::string, std::hash<std::string_view>, std::equal_to<>> stop_words_;
  friend auto operator>>(std::istream &is, StopWordsDictionary &stop_words) -> std::istream &;
  friend auto operator>>(bistream &is, StopWordsDictionary &stop_words) -> bistream &;
  friend auto operator<<(bostream &os, const StopWordsDictionary &stop_words) -> bostream &;

 public:
  /// @brief Test whether @p str is a stop word.
  /// @param str The token to test.
  /// @return true if the token is a stop word.
  auto operator[](std::string_view str) const -> bool;
};

}  // namespace ccjieba
