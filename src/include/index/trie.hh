/// @file trie.hh
/// @brief Unicode trie data structure mapping char32_t phrases to Info entries.

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

/// @brief Value stored at each trie node: log-scaled word frequency and POS tag.
struct Info {
  double weight;  ///< Log-scaled word frequency relative to total corpus frequency.
  std::string tag; ///< Part-of-speech tag (e.g. "n", "v", "a").
};

class UserTrie;

/// @brief Unicode prefix trie (keyed by char32_t) for dictionary lookup and DAG construction.
///
/// Stores phrases with log-probability weights and POS tags. Supports:
/// - Exact prefix match via match()
/// - DAG-building search() for max-probability segmentation
/// - DFS iteration over all entries via const_iterator
/// - User-defined dictionary insertion via UserTrie
///
/// Weights are log-transformed during text deserialization so that path probabilities
/// can be summed instead of multiplied.
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

  /// @brief Insert or update a phrase, returning a pointer to its Info.
  auto upsert(std::u32string_view phrase, Info info) -> Info *;

 public:
  class const_iterator;  // NOLINT(readability-identifier-naming)

  Trie()
    : max_word_length_(0)
    , minimum_weight_(std::numeric_limits<double>::max())
    , maximum_weight_(std::numeric_limits<double>::min()) {}

  /// @brief Exact prefix lookup. Returns null if no prefix of @p str is in the trie.
  auto match(std::u32string_view str) const -> const Info *;

  /// @brief Build a DAG of all dictionary matches starting at each position.
  /// @param str The input string.
  /// @param max_len Maximum match length.
  /// @return A DiGraph with an edge (i, j, Info*) for each match str[i..j].
  auto search(std::u32string_view str, size_t max_len) const -> Graph;

  /// @brief Check whether a character is the start of any user-dictionary entry.
  inline auto in_userdict(char32_t ch) const -> bool { return user_char_.find(ch) != user_char_.end(); }

  /// @brief Minimum log-weight among all entries (used as fallback for unknown words).
  inline auto minimum_weight() const -> double { return minimum_weight_; }

  /// @brief Maximum log-weight among all entries.
  inline auto maximum_weight() const -> double { return maximum_weight_; }

  /// @brief Length of the longest phrase in the trie.
  inline auto max_word_length() const -> size_t { return max_word_length_; }

  /// @brief Begin iterator over all (phrase, Info) entries (DFS order).
  auto begin() const -> const_iterator;

  /// @brief Past-the-end iterator.
  auto end() const -> const_iterator;

  /// @brief Get a UserTrie handle for inserting user-defined words.
  auto user() -> UserTrie;

 private:
  size_t max_word_length_;
  size_t total_frequence_;
  double minimum_weight_;
  double maximum_weight_;
  std::unordered_map<char32_t, Node> root_;
  std::unordered_set<char32_t> user_char_;
};

/// @brief DFS const_iterator over all (phrase, Info*) pairs in the trie.
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

/// @brief Handle for loading user-defined dictionary entries into a Trie.
///
/// Acquired via Trie::user(). Loading via `std::ifstream >> trie.user()` marks
/// the first character of each entry so that MixSegment does not apply HMM
/// segmentation to those positions.
class UserTrie {
  friend Trie;
  friend auto operator>>(std::istream &is, UserTrie &&trie) -> std::istream &;
  Trie &trie_;
  explicit UserTrie(Trie &trie) : trie_(trie) {}
};

}  // namespace ccjieba
