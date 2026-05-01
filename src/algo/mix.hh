#pragma once

#include <cstddef>
#include <string_view>
#include <vector>

#include "index/hmmodel.hh"
#include "index/trie.hh"

namespace ccjieba {

class MixSegment {
  const HMModel &hmm_;
  const Trie &trie_;
  size_t max_word_length_;

 public:
  MixSegment() = delete;
  MixSegment(const HMModel &hmm, const Trie &trie, size_t max_word_length);

  auto operator()(std::u32string_view str) const -> std::vector<std::u32string_view>;
};

}  // namespace ccjieba
