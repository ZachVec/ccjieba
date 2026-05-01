#include "algo/mp.hh"

#include <cstddef>
#include <string_view>

#include "algo/internal.hh"

namespace ccjieba {

MPSegment::MPSegment(const Trie &trie, size_t max_word_length) : trie_(trie), max_word_length_(max_word_length) {}

auto MPSegment::operator()(std::u32string_view str) const -> std::vector<std::u32string_view> {
  auto graph = trie_.search(str, max_word_length_);
  auto segments = mp(graph, trie_.minimum_weight());
  std::vector<std::u32string_view> ret;
  ret.reserve(segments.size());
  for (size_t u = 0, v = 0; u < segments.size(); u = v) {
    v = std::get<0>(segments[u]) + 1;
    ret.push_back(str.substr(u, v - u));
  }
  ret.shrink_to_fit();
  return ret;
}

auto mp(const Graph &graph, double min_weight) -> Segments {
  auto value = Segment(std::u32string_view::npos, -3.14e+100, nullptr);
  Segments segments(graph.size(), value);
  for (ptrdiff_t u = static_cast<ptrdiff_t>(graph.size()) - 1; u >= 0; --u) {
    auto &[indice_of_max, weight_of_max, info_of_max] = segments[u];
    for (const auto &[v, info] : graph[u]) {
      double weight = info != nullptr ? info->weight : min_weight;
      if (v + 1 < segments.size()) {
        weight += std::get<1>(segments[v + 1]);
      }
      if (weight > weight_of_max) {
        weight_of_max = weight;
        indice_of_max = v;
        info_of_max = info;
      }
    }
  }
  return segments;
}

}  // namespace ccjieba
