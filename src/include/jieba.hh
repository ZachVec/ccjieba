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

struct Jieba {
  HMModel hmm_;
  Trie trie_;
  InverseDocumentFrequence idf_;
  StopWordsDictionary stop_words_;

  template <typename Algo = MixSegment>
  auto cut(std::string_view str, std::optional<size_t> max_word_length = std::nullopt) const
      -> std::vector<std::string>;

  auto extract(std::string_view str, size_t top_n) const -> std::vector<Keyword>;

  auto tag_word(std::string_view str) const -> std::string_view;

  auto tag_sentence(std::string_view str) const -> std::vector<std::pair<std::string, std::string_view>>;
};

}  // namespace ccjieba
