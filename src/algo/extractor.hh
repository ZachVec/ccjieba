#pragma once

#include <cstddef>
#include <string>
#include <vector>

#include "index/idf.hh"
#include "index/stop_words.hh"
#include "keyword.hh"

namespace ccjieba {

class KeywordExtractor {
  const InverseDocumentFrequence &idf_;
  const StopWordsDictionary &stop_words_;

 public:
  KeywordExtractor(const InverseDocumentFrequence &idf, const StopWordsDictionary &stop_words)
    : idf_(idf), stop_words_(stop_words) {}

  auto operator()(const std::vector<std::string> &segments, size_t top_n) const -> std::vector<Keyword>;
};

}  // namespace ccjieba
