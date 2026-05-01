#include "index/idf.hh"

#include <cstddef>
#include <functional>
#include <ios>
#include <iostream>
#include <string>
#include <string_view>
#include <utility>

#include "utils/io.hh"
#include "utils/str_utils.hh"

using std::operator""sv;

namespace ccjieba {

auto InverseDocumentFrequence::operator[](std::string_view str) const -> double {
  auto key_eq = idf_.key_eq();
  size_t bucket_id = idf_.hash_function()(str) % idf_.bucket_count();
  for (auto it = idf_.begin(bucket_id); it != idf_.end(bucket_id); ++it) {
    if (key_eq(it->first, str)) {
      return it->second;
    }
  }
  return average_;
}

auto operator>>(std::istream &is, InverseDocumentFrequence &holder) -> std::istream & {
  std::string line;
  double total = 0.0;
  size_t count = 0;
  while (std::getline(is, line)) {
    if (line.empty()) {
      continue;
    }
    auto buf = split(std::string_view(line), " "sv);
    if (buf.size() != 2) {
      is.setstate(std::ios_base::badbit);
      return is;
    }
    std::string word(buf[0]);
    double freq = std::stod(std::string(buf[1]));
    if (not holder.idf_.try_emplace(std::move(word), freq).second) {
      is.setstate(std::ios_base::badbit);
      return is;
    }
    total += freq;
    count += 1;
  }
  if (count > 0) {
    holder.average_ = total / static_cast<double>(count);
  } else {
    holder.average_ = 0;
  }
  return is;
}

auto operator>>(bistream &is, InverseDocumentFrequence &holder) -> bistream & {
  size_t size;
  if (not(is >> size >> holder.average_)) {
    return is;
  }
  std::string word;
  double freq;
  holder.idf_.reserve(size);
  for (size_t i = 0; i < size && is >> word >> freq; ++i) {
    holder.idf_.try_emplace(std::move(word), freq);
  }
  return is;
}

auto operator<<(bostream &os, const InverseDocumentFrequence &holder) -> bostream & {
  os << holder.idf_.size() << holder.average_;
  for (const auto &[k, v] : holder.idf_) {
    os << k << v;
  }
  return os;
}

}  // namespace ccjieba
