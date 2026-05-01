#pragma once

#include <cstddef>
#include <istream>
#include <iterator>
#include <limits>
#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#include "utils/io.hh"

namespace ccjieba {

template <typename T>
class DiGraph;

struct Info {
  double weight;
  std::string tag;
};

class UserTrie;

class Trie {
  using Graph = DiGraph<const Info *>;
  friend class UserTrie;

  struct Node {
    std::unique_ptr<Info> info = nullptr;
    std::unique_ptr<std::unordered_map<char32_t, Node>> next = nullptr;
  };

  friend auto operator>>(std::istream &is, Trie &trie) -> std::istream &;
  friend auto operator>>(bistream &is, Trie &trie) -> bistream &;
  friend auto operator<<(bostream &os, const Trie &trie) -> bostream &;
  friend auto operator>>(std::istream &is, UserTrie &&trie) -> std::istream &;

  auto upsert(std::u32string_view phrase, Info info) -> Info *;

 public:
  class const_iterator;  // NOLINT(readability-identifier-naming)

  Trie()
    : max_word_length_(0)
    , minimum_weight_(std::numeric_limits<double>::max())
    , maximum_weight_(std::numeric_limits<double>::min()) {}

  auto match(std::u32string_view str) const -> const Info *;

  auto search(std::u32string_view str, size_t max_len) const -> Graph;

  inline auto in_userdict(char32_t ch) const -> bool { return user_char_.find(ch) != user_char_.end(); }

  inline auto minimum_weight() const -> double { return minimum_weight_; }

  inline auto maximum_weight() const -> double { return maximum_weight_; }

  inline auto max_word_length() const -> size_t { return max_word_length_; }

  auto begin() const -> const_iterator;

  auto end() const -> const_iterator;

  auto user() -> UserTrie;

 private:
  size_t max_word_length_;
  size_t total_frequence_;
  double minimum_weight_;
  double maximum_weight_;
  std::unordered_map<char32_t, Node> root_;
  std::unordered_set<char32_t> user_char_;
};

class Trie::const_iterator {
  using citer = std::unordered_map<char32_t, Node>::const_iterator;

 public:
  using value_type = std::pair<std::u32string, const Info *>;
  using difference_type = std::ptrdiff_t;
  using iterator_category = std::input_iterator_tag;
  using pointer = value_type *;
  using reference = value_type &;

  const_iterator() = default;

  const_iterator(const std::unordered_map<char32_t, Node> &root);

  auto operator*() const -> value_type;

  auto operator==(const const_iterator &other) const -> bool { return path_ == other.path_; }

  auto operator!=(const const_iterator &other) const -> bool { return path_ != other.path_; }

  auto operator++() -> const_iterator &;

  auto operator++(int) -> const_iterator {
    auto it = *this;
    ++(*this);
    return it;
  }

 private:
  void advance();
  std::vector<std::pair<citer, citer>> path_;
};

class UserTrie {
  friend Trie;
  friend auto operator>>(std::istream &is, UserTrie &&trie) -> std::istream &;
  Trie &trie_;
  explicit UserTrie(Trie &trie) : trie_(trie) {}
};

}  // namespace ccjieba
