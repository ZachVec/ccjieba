#pragma once

#include <cstddef>
#include <string_view>
#include <vector>

#include "index/trie.hh"

namespace ccjieba {

class MPSegment {
  const Trie &trie_;
  size_t max_word_length_;

 public:
  MPSegment() = delete;
  MPSegment(const Trie &trie, size_t max_word_length);

  auto operator()(std::u32string_view str) const -> std::vector<std::u32string_view>;
};

}  // namespace ccjieba
