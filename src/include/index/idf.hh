#pragma once

#include <functional>
#include <istream>
#include <string>
#include <string_view>
#include <unordered_map>

#include "utils/io.hh"

namespace ccjieba {

class InverseDocumentFrequence {
  double average_;
  std::unordered_map<std::string, double, std::hash<std::string_view>, std::equal_to<>> idf_;
  friend auto operator>>(std::istream &is, InverseDocumentFrequence &idf) -> std::istream &;
  friend auto operator>>(bistream &is, InverseDocumentFrequence &idf) -> bistream &;
  friend auto operator<<(bostream &os, const InverseDocumentFrequence &idf) -> bostream &;

 public:
  auto operator[](std::string_view str) const -> double;
};

}  // namespace ccjieba
