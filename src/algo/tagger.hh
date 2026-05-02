/// @file tagger.hh
/// @brief Part-of-speech tagger using dictionary tags with heuristic fallback.

#pragma once

#include <cctype>
#include <cstddef>
#include <string_view>
#include <utility>
#include <vector>

#include "index/hmmodel.hh"
#include "index/trie.hh"

namespace ccjieba {

/// @brief POS tagger that segments text and assigns tags from dictionary or heuristic rules.
///
/// First segments via MixSegment (MP + HMM). Each word is then looked up in the trie DAG
/// for its dictionary tag. Unknown words are classified heuristically:
/// - All-ASCII numeric → "m", all-ASCII alpha → "eng", otherwise → "x".
class Tagger {
  const HMModel &hmm_;
  const Trie &trie_;
  size_t max_word_length_;

 public:
  static constexpr const char *TAG_UNK = "x";   ///< Unknown / Chinese character tag.
  static constexpr const char *TAG_ENG = "eng"; ///< English word tag.
  static constexpr const char *TAG_NUM = "m";   ///< Numeric token tag.

  /// @brief Heuristically classify a word not found in the dictionary.
  /// @param str The unknown word in UTF-32.
  /// @return TAG_UNK, TAG_ENG, or TAG_NUM.
  static auto classify_unknown_word(std::u32string_view str) -> std::string_view {
    size_t threshold = str.size() / 2;
    size_t eng = 0;
    size_t num = 0;
    for (const char32_t ch : str) {
      if (ch < 0x80) {
        ++eng;
        if (std::isdigit(static_cast<int>(ch))) {
          ++num;
        }
      }
      if (eng >= threshold) {
        break;
      }
    }
    if (eng == 0) {
      return TAG_UNK;
    }
    if (eng == num) {
      return TAG_NUM;
    }
    return TAG_ENG;
  }

  Tagger(const HMModel &hmm, const Trie &trie, size_t max_word_length);

  /// @brief Segment and tag a sentence.
  /// @param str Input UTF-32 string.
  /// @return Vector of (word, tag) pairs.
  auto operator()(std::u32string_view str) const -> std::vector<std::pair<std::u32string_view, std::string_view>>;
};

}  // namespace ccjieba
