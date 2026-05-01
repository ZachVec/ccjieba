#include "algo/query.hh"

#include <algorithm>
#include <cstddef>
#include <string_view>
#include <utility>
#include <vector>

#include "algo/internal.hh"
#include "utils/dag.hh"

namespace ccjieba {

QuerySegment::QuerySegment(const HMModel &model, const Trie &trie, size_t max_word_length, std::vector<size_t> lengths)
  : hmm_(model), trie_(trie), max_word_length_(max_word_length), lengths_(std::move(lengths)) {
  std::sort(lengths_.begin(), lengths_.end());
}

auto QuerySegment::operator()(std::u32string_view str) const -> std::vector<std::u32string_view> {
  DiGraph<const Info *> graph = trie_.search(str, max_word_length_);
  std::vector<std::u32string_view> segments = mix(str, mp(graph, trie_.minimum_weight()), hmm_, trie_);
  auto collect = [&graph, this, &str](size_t u, size_t max_length, std::vector<std::u32string_view> &ret) {
    size_t i = 0;  // index of lengths
    size_t j = 1;  // index of node
    while (i < lengths_.size() and lengths_[i] <= max_length and j < graph[u].size()) {
      size_t length = graph[u][j].first - u + 1;
      if (length > lengths_[i]) {
        ++i;
      } else if (length < lengths_[i]) {
        ++j;
      } else {
        ++i;
        ++j;
        ret.emplace_back(str.substr(u, length));
      }
    }
  };

  std::vector<std::u32string_view> ret;
  ret.reserve(str.size());
  size_t u = 0;
  for (const std::u32string_view &seg : segments) {
    collect(u, seg.size() - 1, ret);
    ret.emplace_back(seg);
    for (size_t offset = 1; offset < seg.size(); ++offset) {
      collect(u + offset, seg.size() - offset, ret);
    }
    u += seg.size();
  }
  ret.shrink_to_fit();
  return ret;
}

}  // namespace ccjieba
