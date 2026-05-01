#include "algo/full.hh"

#include <algorithm>
#include <cstddef>
#include <iterator>
#include <string_view>
#include <vector>

#include "index/trie.hh"
#include "utils/dag.hh"

namespace ccjieba {

FullSegment::FullSegment(const Trie &trie, size_t max_word_length) : trie_(trie), max_word_length_(max_word_length) {}

auto FullSegment::operator()(std::u32string_view str) const -> std::vector<std::u32string_view> {
  const DiGraph<const Info *> &graph = trie_.search(str, max_word_length_);

  size_t unprocessed_index = 0;
  std::vector<std::u32string_view> ret;
  for (auto node = graph.begin(); node != graph.end(); ++node) {
    auto i = static_cast<size_t>(std::distance(graph.begin(), node));
    for (const auto &[j, _] : *node) {
      size_t length = j - i + 1;
      if (length >= 2) {
        ret.push_back(str.substr(i, length));
        unprocessed_index = std::max(unprocessed_index, j + 1);
      } else if (node->size() == 1 and i >= unprocessed_index) {
        ret.push_back(str.substr(i, 1));
        unprocessed_index = std::max(unprocessed_index, j + 1);
      }
    }
  }

  return ret;
}

}  // namespace ccjieba
