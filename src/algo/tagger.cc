/// @file tagger.cc
/// @brief Tagger implementation: MixSegment-based segmentation with dictionary POS tag lookup.

#include "algo/tagger.hh"

#include <algorithm>
#include <cstddef>
#include <string_view>
#include <utility>
#include <vector>

#include "algo/internal.hh"
#include "index/trie.hh"

namespace ccjieba {

/// @brief Comparison helper for binary-searching graph edges by target index.
static auto comp(const Edge &a, const Edge &b) -> bool {
  return a.first < b.first;
}

Tagger::Tagger(const HMModel &hmm, const Trie &trie, size_t max_word_length)
  : hmm_(hmm), trie_(trie), max_word_length_(max_word_length) {}

/// @brief Segment the input then look up each word's tag in the trie DAG.
///
/// For each word produced by MixSegment, binary-searches the DAG edges at its starting
/// position. If an exact match is found, uses the dictionary tag; otherwise falls back
/// to classify_unknown_word().
auto Tagger::operator()(std::u32string_view str) const
    -> std::vector<std::pair<std::u32string_view, std::string_view>> {
  auto graph = trie_.search(str, max_word_length_);
  auto segments = mp(graph, trie_.minimum_weight());
  size_t index = 0;
  std::vector<std::u32string_view> words = mix(str, segments, hmm_, trie_);
  std::vector<std::pair<std::u32string_view, std::string_view>> ret;
  ret.reserve(words.size());

  for (const std::u32string_view &word : words) {
    size_t v = index + word.size() - 1;
    auto it = std::lower_bound(graph[index].begin(), graph[index].end(), Edge{v, nullptr}, comp);
    if (it == graph[index].end() || it->first != v || it->second == nullptr) {
      ret.emplace_back(word, Tagger::classify_unknown_word(word));
    } else {
      ret.emplace_back(word, it->second->tag);
    }
    index += word.size();
  }
  ret.shrink_to_fit();
  return ret;
}

}  // namespace ccjieba
