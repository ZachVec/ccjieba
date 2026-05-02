/// @file extractor.hh
/// @brief TF-IDF keyword extractor returning top-N keywords with offsets.

#pragma once

#include <cstddef>
#include <string>
#include <vector>

#include "index/idf.hh"
#include "index/stop_words.hh"
#include "keyword.hh"

namespace ccjieba {

/// @brief TF-IDF keyword extractor.
///
/// Counts term frequencies from segmented input, multiplies by IDF scores,
/// filters stop words and single characters, then returns the top N by weight.
class KeywordExtractor {
  const InverseDocumentFrequence &idf_;
  const StopWordsDictionary &stop_words_;

 public:
  KeywordExtractor(const InverseDocumentFrequence &idf, const StopWordsDictionary &stop_words)
    : idf_(idf), stop_words_(stop_words) {}

  /// @brief Extract top-N keywords from segmented tokens.
  /// @param segments Pre-segmented UTF-8 tokens (e.g. from Jieba::cut()).
  /// @param top_n Maximum number of keywords to return.
  /// @return Keywords sorted by descending TF-IDF weight.
  auto operator()(const std::vector<std::string> &segments, size_t top_n) const -> std::vector<Keyword>;
};

}  // namespace ccjieba
