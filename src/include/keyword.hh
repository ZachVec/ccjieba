/// @file keyword.hh
/// @brief Keyword result type returned by Jieba::extract().

#pragma once

#include <cstddef>
#include <string>
#include <vector>

namespace ccjieba {

/// @brief A keyword extraction result with TF-IDF weight, surface form, and byte offsets.
struct Keyword {
  double weight;                ///< TF-IDF weight (higher = more significant).
  std::string word;             ///< The keyword surface form in UTF-8.
  std::vector<std::size_t> offsets; ///< Byte offsets in the original input where this word appears.

  auto operator<(const Keyword &other) const -> bool { return weight < other.weight; }

  auto operator>(const Keyword &other) const -> bool { return weight > other.weight; }
};

}  // namespace ccjieba
