#pragma once

#include <cstddef>
#include <utility>
#include <vector>

namespace ccjieba {

template <typename T>
class DiGraph {
  using Node = std::vector<std::pair<size_t, T>>;

 public:
  DiGraph(size_t size) : nodes_(size) {}

  inline void add_edge(size_t u, size_t v, T data) { nodes_[u].emplace_back(v, std::move(data)); }

  inline auto size() const -> size_t { return nodes_.size(); }

  inline auto operator[](size_t index) const -> const Node & { return nodes_[index]; }

  inline auto begin() const -> typename std::vector<Node>::const_iterator { return nodes_.begin(); }

  inline auto end() const -> typename std::vector<Node>::const_iterator { return nodes_.end(); }

  inline auto rbegin() const -> typename std::vector<Node>::const_reverse_iterator { return nodes_.rbegin(); }

  inline auto rend() const -> typename std::vector<Node>::const_reverse_iterator { return nodes_.rend(); }

 private:
  std::vector<Node> nodes_;
};

}  // namespace ccjieba
