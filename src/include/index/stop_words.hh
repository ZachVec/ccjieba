#pragma once

#include <functional>
#include <istream>
#include <string>
#include <string_view>
#include <unordered_set>

#include "utils/io.hh"

namespace ccjieba {

class StopWordsDictionary {
  std::unordered_set<std::string, std::hash<std::string_view>, std::equal_to<>> stop_words_;
  friend auto operator>>(std::istream &is, StopWordsDictionary &stop_words) -> std::istream &;
  friend auto operator>>(bistream &is, StopWordsDictionary &stop_words) -> bistream &;
  friend auto operator<<(bostream &os, const StopWordsDictionary &stop_words) -> bostream &;

 public:
  auto operator[](std::string_view str) const -> bool;
};

}  // namespace ccjieba
