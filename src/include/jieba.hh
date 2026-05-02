/// @file jieba.hh
/// @brief Main entry point for the ccjieba Chinese text segmentation library.
///
/// Provides the Jieba aggregate class that bundles all indices (dictionary, HMM, IDF, stop words)
/// and exposes segmentation, keyword extraction, and POS tagging through a single facade.

#pragma once

#include <cstddef>
#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include "index/hmmodel.hh"
#include "index/idf.hh"
#include "index/stop_words.hh"
#include "index/trie.hh"
#include "keyword.hh"

namespace ccjieba {

class FullSegment;
class HMMSegment;
class MixSegment;
class MPSegment;
class QuerySegment;

/// @brief Aggregate facade owning all indices and dispatching segmentation requests.
///
/// Load each index from text data via `std::ifstream >> member` before use.
/// The default segmenter is MixSegment (dictionary with HMM fallback).
///
/// Usage:
/// @code
/// Jieba jieba;
/// std::ifstream("dict.utf8") >> jieba.trie_;
/// std::ifstream("hmm.utf8") >> jieba.hmm_;
/// std::ifstream("idf.utf8") >> jieba.idf_;
/// auto words = jieba.cut("我爱北京天安门");
/// @endcode
struct Jieba {
  HMModel hmm_;               ///< Hidden Markov Model for unknown-word segmentation.
  Trie trie_;                  ///< Dictionary trie with log-probability weights and POS tags.
  InverseDocumentFrequence idf_; ///< IDF scores for keyword extraction ranking.
  StopWordsDictionary stop_words_; ///< Stop-words filtered out during keyword extraction.

  /// @brief Segment text using the given algorithm.
  /// @tparam Algo Segmenter type — one of MPSegment, HMMSegment, MixSegment (default), FullSegment, QuerySegment.
  /// @param str UTF-8 input string.
  /// @param max_word_length Maximum dictionary word length to consider (default 500).
  /// @return Vector of segmented UTF-8 words.
  template <typename Algo = MixSegment>
  auto cut(std::string_view str, std::optional<size_t> max_word_length = std::nullopt) const
      -> std::vector<std::string>;

  /// @brief Extract top-N keywords by TF-IDF weight.
  /// @param str UTF-8 input string.
  /// @param top_n Number of keywords to return.
  /// @return Vector of keywords sorted by descending weight.
  auto extract(std::string_view str, size_t top_n) const -> std::vector<Keyword>;

  /// @brief Classify a single word's POS tag.
  /// @param str UTF-8 word.
  /// @return Tag string (e.g. "n", "v", "x" for unknown).
  auto tag_word(std::string_view str) const -> std::string_view;

  /// @brief Segment and POS-tag every word in a sentence.
  /// @param str UTF-8 input sentence.
  /// @return Vector of (word, tag) pairs.
  auto tag_sentence(std::string_view str) const -> std::vector<std::pair<std::string, std::string_view>>;
};

}  // namespace ccjieba
