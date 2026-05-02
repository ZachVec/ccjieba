/// @file idf.hh
/// @brief Inverse Document Frequency index for keyword extraction scoring.

#pragma once

#include <functional>
#include <istream>
#include <string>
#include <string_view>
#include <unordered_map>

#include "utils/io.hh"

namespace ccjieba {

/// @brief Inverse Document Frequency lookup table.
///
/// Maps tokens to IDF scores. Unknown tokens fall back to the average IDF across all entries.
/// Uses heterogeneous hash/equality for `string_view` lookup without allocation.
class InverseDocumentFrequence {
  double average_;
  std::unordered_map<std::string, double, std::hash<std::string_view>, std::equal_to<>> idf_;
  friend auto operator>>(std::istream &is, InverseDocumentFrequence &idf) -> std::istream &;
  friend auto operator>>(bistream &is, InverseDocumentFrequence &idf) -> bistream &;
  friend auto operator<<(bostream &os, const InverseDocumentFrequence &idf) -> bostream &;

 public:
  /// @brief Look up IDF score for a token.
  /// @param str The token to look up.
  /// @return IDF score, or the average IDF if the token is unknown.
  auto operator[](std::string_view str) const -> double;
};

}  // namespace ccjieba
