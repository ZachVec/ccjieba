/// @file query.hh
/// @brief Query-oriented segmenter: MixSegment output plus finer-grained sub-segments.

#pragma once

#include <cstddef>
#include <string_view>
#include <vector>

#include "index/hmmodel.hh"
#include "index/trie.hh"

namespace ccjieba {

/// @brief Query-oriented segmenter that adds sub-segments at configurable lengths.
///
/// First runs MixSegment (MP + HMM), then for each output word, also emits dictionary
/// substrings of lengths specified in @p lengths (default {2, 3}). This increases recall
/// for search queries where both full words and character n-grams should match.
class QuerySegment {
  const HMModel &hmm_;
  const Trie &trie_;
  size_t max_word_length_;
  std::vector<size_t> lengths_;

 public:
  QuerySegment() = delete;
  /// @param model HMModel reference.
  /// @param trie Trie reference.
  /// @param max_word_length Maximum dictionary word length.
  /// @param lengths Sub-segment lengths to emit (sorted on construction). Default: {2, 3}.
  QuerySegment(const HMModel &model, const Trie &trie, size_t max_word_length, std::vector<size_t> lengths = {2, 3});

  /// @brief Segment with sub-segmentation.
  /// @param str Input string.
  /// @return Vector of u32string_views including main segments and sub-segments.
  auto operator()(std::u32string_view str) const -> std::vector<std::u32string_view>;
};

}  // namespace ccjieba
