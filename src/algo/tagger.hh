#pragma once

#include <cctype>
#include <cstddef>
#include <string_view>
#include <utility>
#include <vector>

#include "index/hmmodel.hh"
#include "index/trie.hh"

namespace ccjieba {

class Tagger {
  const HMModel &hmm_;
  const Trie &trie_;
  size_t max_word_length_;

 public:
  static constexpr const char *TAG_UNK = "x";
  static constexpr const char *TAG_ENG = "eng";
  static constexpr const char *TAG_NUM = "m";

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

  auto operator()(std::u32string_view str) const -> std::vector<std::pair<std::u32string_view, std::string_view>>;
};

}  // namespace ccjieba
