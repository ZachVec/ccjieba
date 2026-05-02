/// @file internal.hh
/// @brief Shared types and free functions used internally by segmenters.

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

/// @brief A segment choice: (end_index, cumulative_weight, dictionary_info_ptr).
///
/// Used by mp() to record the optimal segmentation path through the DAG.
using Segment = std::tuple<size_t, double, const Info *>;
/// @brief Vector of Segment, indexed by starting position in the sentence.
using Segments = std::vector<Segment>;
/// @brief DAG where each edge represents a dictionary match with its Info payload.
using Graph = DiGraph<const Info *>;
/// @brief An edge in the DAG: (target_index, dictionary_info_ptr).
using Edge = std::pair<size_t, const Info *>;

/// @brief Max-probability segmentation over a trie DAG via reverse dynamic programming.
/// @param graph The DAG produced by Trie::search().
/// @param min_weight Fallback weight for single-character segments (unknown characters).
/// @return Optimal Segment at each starting position.
auto mp(const Graph &graph, double min_weight) -> Segments;

/// @brief Hybrid segmentation: apply HMM to spans not covered by the dictionary.
/// @param str The input string.
/// @param segs The dictionary segments from mp().
/// @param model The HMModel for unknown-word segmentation.
/// @param trie The Trie (used to check user-dict membership).
/// @return A mix of dictionary words and HMM-segmented spans.
auto mix(std::u32string_view str, const Segments &segs, const HMModel &model, const Trie &trie)
    -> std::vector<std::u32string_view>;

}  // namespace ccjieba
