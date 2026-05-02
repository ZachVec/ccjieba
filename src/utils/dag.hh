/// @file dag.hh
/// @brief Minimal directed graph for representing trie match DAGs.

#pragma once

#include <cstddef>
#include <utility>
#include <vector>

namespace ccjieba {

/// @brief Adjacency-list directed graph with edge payloads.
///
/// Nodes are indexed 0..size()-1. Each edge carries an arbitrary payload of type T
/// (in practice, `const Info*` from the trie). Graph is built once per sentence
/// via Trie::search() and consumed by segmenters.
///
/// @tparam T Payload type stored on each edge.
template <typename T>
class DiGraph {
  using Node = std::vector<std::pair<size_t, T>>;

 public:
  DiGraph(size_t size) : nodes_(size) {}

  /// @brief Add a directed edge u → v with payload data.
  inline void add_edge(size_t u, size_t v, T data) { nodes_[u].emplace_back(v, std::move(data)); }

  /// @brief Number of nodes in the graph.
  inline auto size() const -> size_t { return nodes_.size(); }

  /// @brief Get the adjacency list for node @p index.
  inline auto operator[](size_t index) const -> const Node & { return nodes_[index]; }

  inline auto begin() const -> typename std::vector<Node>::const_iterator { return nodes_.begin(); }

  inline auto end() const -> typename std::vector<Node>::const_iterator { return nodes_.end(); }

  /// @brief Reverse begin (for reverse DP traversal).
  inline auto rbegin() const -> typename std::vector<Node>::const_reverse_iterator { return nodes_.rbegin(); }

  /// @brief Reverse end.
  inline auto rend() const -> typename std::vector<Node>::const_reverse_iterator { return nodes_.rend(); }

 private:
  std::vector<Node> nodes_;
};

}  // namespace ccjieba
