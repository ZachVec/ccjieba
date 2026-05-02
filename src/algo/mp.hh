/// @file mp.hh
/// @brief Max-probability segmenter: optimal segmentation via DP over the trie DAG.

#pragma once

#include <cstddef>
#include <string_view>
#include <vector>

#include "index/trie.hh"

namespace ccjieba {

/// @brief Dictionary-only max-probability segmenter.
///
/// Builds a DAG from the trie, then runs reverse dynamic programming (the mp() free function)
/// to select the single highest-weight path. Produces the minimum-cut segmentation
/// according to the dictionary's log-probability weights.
class MPSegment {
  const Trie &trie_;
  size_t max_word_length_;

 public:
  MPSegment() = delete;
  MPSegment(const Trie &trie, size_t max_word_length);

  /// @brief Segment a UTF-32 string using dictionary max-probability.
  /// @param str Input string.
  /// @return Vector of non-overlapping u32string_views into @p str.
  auto operator()(std::u32string_view str) const -> std::vector<std::u32string_view>;
};

}  // namespace ccjieba
