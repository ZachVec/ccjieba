/// @file extractor.cc
/// @brief KeywordExtractor implementation: TF-IDF scoring with stop-word filtering.

#include "algo/extractor.hh"

#include <algorithm>
#include <cstddef>
#include <functional>
#include <string>
#include <string_view>
#include <tuple>
#include <unordered_map>
#include <utility>
#include <vector>

#include "keyword.hh"

namespace {

/// @brief Test whether a UTF-8 string is a single character.
///
/// Determines this by checking if the byte length of the first UTF-8 code point
/// equals the total string length (no multi-character words passed through).
inline auto is_one_letter_word(std::string_view str) -> bool {
  auto c = static_cast<unsigned char>(str.front());
  if (c < 0x80) {
    return str.size() == 1;
  }
  size_t len = 0;
  for (unsigned char mask = 0x80; c & mask; mask >>= 1) {
    ++len;
  }
  return len == str.size();
}

}  // namespace

namespace ccjieba {

/// @brief Count term frequencies, multiply by IDF, filter, and return top N.
///
/// Single-character words and stop-words are excluded. Terms are scored by
/// `term_frequency * idf[term]` and the top @p top_n are returned sorted
/// by descending weight.
auto KeywordExtractor::operator()(const std::vector<std::string> &segments, size_t top_n) const
    -> std::vector<Keyword> {
  std::unordered_map<std::string_view, std::tuple<size_t, std::vector<size_t>>> keywords;
  size_t offset = 0;
  for (const auto &segment : segments) {
    if (is_one_letter_word(segment) or stop_words_[segment]) {
      offset += segment.size();
      continue;
    }
    auto &[count, offsets] = keywords[segment];
    ++count;
    offsets.push_back(offset);
    offset += segment.size();
  }
  std::vector<Keyword> ret;
  ret.reserve(keywords.size());
  for (auto &[word, value] : keywords) {
    auto weight = static_cast<double>(std::get<0>(value));
    double req = idf_[word];
    ret.push_back({
        .weight = weight * req,
        .word = std::string(word.data(), word.length()),
        .offsets = std::move(std::get<1>(value)),
    });
  }
  top_n = std::min(top_n, keywords.size());
  std::partial_sort(ret.begin(), ret.begin() + static_cast<ptrdiff_t>(top_n), ret.end(), std::greater<Keyword>());
  ret.resize(top_n);
  return ret;
}

}  // namespace ccjieba
