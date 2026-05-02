/// @file trie.cc
/// @brief Trie implementation: upsert, match, search, I/O, and DFS const_iterator.

#include "index/trie.hh"

#include <algorithm>
#include <charconv>
#include <cmath>
#include <cstddef>
#include <cstdlib>
#include <ios>
#include <iostream>
#include <istream>
#include <limits>
#include <memory>
#include <string>
#include <string_view>
#include <system_error>
#include <unordered_map>
#include <utility>
#include <vector>

#include "utils/dag.hh"
#include "utils/io.hh"
#include "utils/str_utils.hh"

using std::operator""sv;

namespace ccjieba {

/// @brief Insert or update a phrase in the trie.
///
/// Walks the trie path for the phrase, creating missing nodes along the way,
/// and sets the Info at the final node. Returns a pointer to the stored Info.
auto Trie::upsert(std::u32string_view phrase, Info info) -> Info * {
  size_t length = phrase.size();
  if (length == 0) {
    return nullptr;
  }

  auto [it, _1] = root_.try_emplace(phrase.front());
  for (size_t i = 1; i < length; ++i) {
    auto &[_2, next] = it->second;
    if (next == nullptr) {
      next = std::make_unique<std::unordered_map<char32_t, Node>>();
    }
    it = next->try_emplace(phrase[i]).first;
  }
  it->second.info = std::make_unique<Info>(std::move(info));
  return it->second.info.get();
}

/// @brief Exact prefix match: walk the trie following @p str characters.
///
/// Returns the last non-null Info encountered, or nullptr if the path is not in the trie.
auto Trie::match(std::u32string_view str) const -> const Info * {
  Info *info = nullptr;
  const std::unordered_map<char32_t, Node> *next = &root_;
  for (const char32_t c : str) {
    if (next == nullptr) {
      return nullptr;
    }
    auto it = next->find(c);
    if (it == next->end()) {
      return nullptr;
    }
    info = it->second.info.get();
    next = it->second.next.get();
  }
  return info;
}

/// @brief Build a DAG of all dictionary matches for every starting position.
///
/// For each position i, follows the trie path up to max_len characters and adds
/// an edge (i, j) for each node that has a non-null Info.
auto Trie::search(std::u32string_view str, size_t max_len) const -> Graph {
  size_t size = str.size();

  DiGraph<const Info *> graph(size);
  for (size_t i = 0; i < size; ++i) {
    const std::unordered_map<char32_t, Node> *next = &root_;
    auto it = next->find(str[i]);
    const Info *du = nullptr;
    if (it == next->end()) {
      next = nullptr;
    } else {
      next = it->second.next.get();
      du = it->second.info.get();
    }
    graph.add_edge(i, i, du);

    size_t max_j = std::min(size, i + max_len);
    for (size_t j = i + 1; j < max_j; ++j) {
      if (next == nullptr) {
        break;
      }
      it = next->find(str[j]);
      if (it == next->end()) {
        break;
      }
      if (it->second.info != nullptr) {
        graph.add_edge(i, j, it->second.info.get());
      }
      next = it->second.next.get();
    }
  }
  return graph;
}

auto Trie::begin() const -> const_iterator {
  return const_iterator(root_);
}

auto Trie::end() const -> const_iterator {
  return const_iterator();
}

auto Trie::user() -> UserTrie {
  return UserTrie(*this);
}

/// @brief Load dictionary from text format: one entry per line as "phrase freq tag".
///
/// Frequencies are log-transformed relative to total_frequence_ so that
/// path probabilities can be summed in the Viterbi and DP algorithms.
auto operator>>(std::istream &is, Trie &trie) -> std::istream & {
  size_t total_freq = 0;
  size_t min_freq = std::numeric_limits<size_t>::max();
  size_t max_freq = std::numeric_limits<size_t>::min();
  std::vector<Info *> infos;
  for (std::string line; std::getline(is, line);) {
    std::string_view view = strip(std::string_view(line), " \n"sv);
    if (view.empty()) {
      continue;
    }
    auto segs = split(view, " "sv);
    if (segs.size() != 3) {
      is.setstate(std::ios_base::badbit);
      return is;
    }

    std::u32string phrase;
    if (not utf8_to_utf32(segs[0], phrase)) {
      is.setstate(std::ios_base::badbit);
      continue;
    }

    size_t frequence;
    auto [_, ec] = std::from_chars(segs[1].begin(), segs[1].end(), frequence);
    if (ec != std::errc{}) {
      is.setstate(std::ios_base::badbit);
      continue;
    }

    std::string tag(segs[2]);
    Info *info = trie.upsert(phrase, {.weight = static_cast<double>(frequence), .tag = std::move(tag)});
    if (info != nullptr) {
      total_freq += frequence;
      min_freq = std::min(min_freq, frequence);
      max_freq = std::max(max_freq, frequence);
      infos.push_back(info);
    }
  }
  trie.maximum_weight_ = std::log(static_cast<double>(max_freq) / static_cast<double>(total_freq));
  trie.minimum_weight_ = std::log(static_cast<double>(min_freq) / static_cast<double>(total_freq));
  trie.total_frequence_ = total_freq;
  for (Info *info : infos) {
    info->weight = std::log(info->weight / static_cast<double>(total_freq));
  }
  return is;
}

/// @brief Binary-serialise the trie: total freq, min/max weight, then each (phrase, weight, tag).
auto operator<<(bostream &os, const Trie &trie) -> bostream & {
  os << trie.total_frequence_ << trie.minimum_weight_ << trie.maximum_weight_;
  for (const auto &[phrase, info] : trie) {
    os << phrase << info->weight << info->tag;
  }
  return os;
}

/// @brief Binary-deserialise the trie with pre-computed log weights.
auto operator>>(bistream &is, Trie &trie) -> bistream & {
  if (not(is >> trie.total_frequence_ >> trie.minimum_weight_ >> trie.maximum_weight_)) {
    return is;
  }
  std::u32string phrase;
  double weight;
  std::string tag;
  while (is >> phrase >> weight >> tag) {
    trie.upsert(phrase, {.weight = weight, .tag = std::move(tag)});
  }
  return is;
}

Trie::const_iterator::const_iterator(const std::unordered_map<char32_t, Node> &root) {
  path_.emplace_back(root.begin(), root.end());
  advance();
}

/// @brief Advance to the next node carrying a non-null Info, using DFS.
void Trie::const_iterator::advance() {
  while (not path_.empty()) {
    auto &[it, end] = path_.back();
    for (; it != end; ++it) {
      const auto &[info, next] = it->second;
      if (next != nullptr) {
        path_.emplace_back(next->begin(), next->end());
        break;
      }
      if (info != nullptr) {
        return;
      }
    }
    if (it == end) {
      path_.pop_back();
      if (not path_.empty()) {
        if (path_.back().first->second.info != nullptr) {
          return;
        }
        ++path_.back().first;
      }
    }
  }
}

auto Trie::const_iterator::operator*() const -> value_type {
  std::u32string s;
  s.reserve(path_.size());
  for (const auto &[it, _] : path_) {
    s.push_back(it->first);
  }
  return std::make_pair(std::move(s), path_.back().first->second.info.get());
}

auto Trie::const_iterator::operator++() -> const_iterator & {
  if (not path_.empty()) {
    ++path_.back().first;
    advance();
  }
  return *this;
}

/// @brief Load user dictionary from text format.
///
/// Supports 1, 2, or 3 columns: "phrase", "phrase tag", or "phrase freq tag".
/// Marks the first character of each entry in user_char_ so that MixSegment
/// preserves these positions instead of applying HMM.
auto operator>>(std::istream &is, UserTrie &&trie) -> std::istream & {
  for (std::string line; std::getline(is, line);) {
    std::string_view view = strip(std::string_view(line), " \n"sv);
    if (view.empty()) {
      continue;
    }
    auto segs = split(view, " "sv);
    std::u32string phrase;
    Info info{.weight = trie.trie_.minimum_weight(), .tag = {}};
    if (not utf8_to_utf32(segs.front(), phrase)) {
      is.setstate(std::ios_base::badbit);
      return is;
    }
    if (segs.size() >= 2) {
      info.tag = std::string(segs.back().data(), segs.back().size());
    }
    if (segs.size() == 3) {
      size_t freq;
      auto [_, ec] = std::from_chars(segs[1].begin(), segs[1].end(), freq);
      if (ec != std::errc{}) {
        is.setstate(std::ios_base::badbit);
        return is;
      }
      info.weight = std::log(static_cast<double>(freq) / static_cast<double>(trie.trie_.total_frequence_));
    }
    trie.trie_.upsert(phrase, std::move(info));
    trie.trie_.user_char_.insert(phrase.front());
  }
  return is;
}

}  // namespace ccjieba
