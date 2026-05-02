/// @file mix.hh
/// @brief Default hybrid segmenter: dictionary MP + HMM fallback for unknown words.

#pragma once

#include <cstddef>
#include <string_view>
#include <vector>

#include "index/hmmodel.hh"
#include "index/trie.hh"

namespace ccjieba {

/// @brief Hybrid segmenter combining max-probability dictionary segmentation with HMM fallback.
///
/// First runs mp() on the trie DAG to find dictionary words. Then passes contiguous spans
/// of single-character unknowns through HMMSegment, preserving user-dictionary positions.
/// This is the default algorithm used by Jieba::cut().
class MixSegment {
  const HMModel &hmm_;
  const Trie &trie_;
  size_t max_word_length_;

 public:
  MixSegment() = delete;
  MixSegment(const HMModel &hmm, const Trie &trie, size_t max_word_length);

  /// @brief Segment a UTF-32 string using the hybrid algorithm.
  /// @param str Input string.
  /// @return Vector of non-overlapping u32string_views.
  auto operator()(std::u32string_view str) const -> std::vector<std::u32string_view>;
};

}  // namespace ccjieba
