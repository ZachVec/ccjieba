#pragma once

#include <cstddef>
#include <string_view>
#include <vector>

#include "index/hmmodel.hh"
#include "index/trie.hh"

namespace ccjieba {

class QuerySegment {
  const HMModel &hmm_;
  const Trie &trie_;
  size_t max_word_length_;
  std::vector<size_t> lengths_;

 public:
  QuerySegment() = delete;
  QuerySegment(const HMModel &model, const Trie &trie, size_t max_word_length, std::vector<size_t> lengths = {2, 3});

  auto operator()(std::u32string_view str) const -> std::vector<std::u32string_view>;
};

}  // namespace ccjieba
