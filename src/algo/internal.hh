#pragma once

#include <cstddef>
#include <string_view>
#include <tuple>
#include <utility>
#include <vector>

#include "index/trie.hh"
#include "utils/dag.hh"

namespace ccjieba {

class HMModel;

using Segment = std::tuple<size_t, double, const Info *>;
using Segments = std::vector<Segment>;
using Graph = DiGraph<const Info *>;
using Edge = std::pair<size_t, const Info *>;

auto mp(const Graph &graph, double min_weight) -> Segments;
auto mix(std::u32string_view str, const Segments &segs, const HMModel &model, const Trie &trie)
    -> std::vector<std::u32string_view>;

}  // namespace ccjieba
