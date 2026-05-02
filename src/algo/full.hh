/// @file full.hh
/// @brief Full segmenter: returns all dictionary words found in the input.

#pragma once

#include <cstddef>
#include <string_view>
#include <vector>

#include "index/trie.hh"

namespace ccjieba {

/// @brief Returns ALL possible dictionary matches for every position, not just the optimal path.
///
/// Useful for query expansion or highlighting. For positions with only one single-character
/// edge, emits that single character (covers characters not in the dictionary).
class FullSegment {
  const Trie &trie_;
  size_t max_word_length_;

 public:
  FullSegment() = delete;
  FullSegment(const Trie &trie, size_t max_word_length);

  /// @brief Extract all dictionary matches.
  /// @param str Input string.
  /// @return Vector of overlapping u32string_views for every dictionary word found.
  auto operator()(std::u32string_view str) const -> std::vector<std::u32string_view>;
};

}  // namespace ccjieba
