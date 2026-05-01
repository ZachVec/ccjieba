#pragma once

#include <cstddef>
#include <string>
#include <vector>

namespace ccjieba {

struct Keyword {
  double weight;
  std::string word;
  std::vector<std::size_t> offsets;

  auto operator<(const Keyword &other) const -> bool { return weight < other.weight; }

  auto operator>(const Keyword &other) const -> bool { return weight > other.weight; }
};

}  // namespace ccjieba
