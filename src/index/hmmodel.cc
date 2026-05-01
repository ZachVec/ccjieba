#include "index/hmmodel.hh"

#include <array>
#include <charconv>
#include <cstddef>
#include <ios>
#include <iostream>
#include <optional>
#include <string>
#include <string_view>

#include "utils/io.hh"
#include "utils/str_utils.hh"

using std::operator""sv;

namespace ccjieba {

template <typename T, size_t N>
static auto operator<<(bostream &os, const std::array<T, N> &array) -> bostream & {
  return os.dump(reinterpret_cast<const std::byte *>(array.data()), array.size() * sizeof(T));
}

template <typename T, size_t N>
static auto operator>>(bistream &is, std::array<T, N> &array) -> bistream & {
  return is.read(reinterpret_cast<std::byte *>(array.data()), sizeof(T) * N);
}

template <typename K, typename V>
static auto operator<<(bostream &os, const std::unordered_map<K, V> &map) -> bostream & {
  os.reserve(os.size() + map.size() * (sizeof(K) + sizeof(V)) + sizeof(size_t));
  os << map.size();
  for (const auto &[k, v] : map) {
    os << k << v;
  }
  return os;
}

template <typename K, typename V>
static auto operator>>(bistream &is, std::unordered_map<K, V> &map) -> bistream & {
  size_t size;
  if (not(is >> size)) {
    return is;
  }
  K k{};
  V v{};
  for (size_t i = 0; i < size; ++i) {
    if (not(is >> k) or not(is >> v)) {
      return is;
    }
    map[std::move(k)] = std::move(v);
  }
  return is;
}

auto operator>>(std::istream &is, HMModel &model) -> std::istream & {
  std::string line;
  auto readline = [&line](std::istream &is) -> std::optional<std::string_view> {
    while (std::getline(is, line)) {
      std::string_view view = strip(std::string_view(line), " \f\n\r\t\v"sv);
      if (view.empty() or view.front() == '#') {
        continue;
      }
      return view;
    }
    return std::nullopt;
  };

  // read pstart
  std::optional<std::string_view> view = readline(is);
  if (not view.has_value()) {
    is.setstate(std::ios_base::badbit);
    return is;
  }
  std::vector<std::string_view> splits = split(view.value(), " "sv);
  if (splits.size() != HMModel::NSTATE) {
    is.setstate(std::ios_base::badbit);
    return is;
  }
  for (size_t i = 0; i < HMModel::NSTATE; ++i) {
    auto [_, ec] = std::from_chars(splits[i].begin(), splits[i].end(), model.pstart_[i]);
    if (ec != std::errc{}) {
      is.setstate(std::ios_base::badbit);
      return is;
    }
  }

  // read ptrans
  for (size_t i = 0; i < HMModel::NSTATE; ++i) {
    view = readline(is);
    if (not view.has_value()) {
      is.setstate(std::ios_base::badbit);
      return is;
    }
    splits = split(view.value(), " "sv);
    if (splits.size() != HMModel::NSTATE) {
      is.setstate(std::ios_base::badbit);
      return is;
    }
    for (size_t j = 0; j < HMModel::NSTATE; ++j) {
      auto [_, ec] = std::from_chars(splits[j].begin(), splits[j].end(), model.ptrans_[i][j]);
      if (ec != std::errc{}) {
        is.setstate(std::ios_base::badbit);
        return is;
      }
    }
  }
  // read pemits
  for (size_t i = 0; i < HMModel::NSTATE; ++i) {
    auto &emit_map = model.pemits_[i];
    view = readline(is);
    if (not view.has_value()) {
      is.setstate(std::ios_base::badbit);
      return is;
    }
    splits = split(view.value(), ","sv);
    for (const auto &term : splits) {
      auto sp = split(term, ":"sv);
      if (sp.size() != 2) {
        is.setstate(std::ios_base::badbit);
        return is;
      }
      std::u32string t;
      if (not utf8_to_utf32(sp[0], t)) {
        is.setstate(std::ios_base::badbit);
        return is;
      }
      auto [_, ec] = std::from_chars(sp[1].begin(), sp[1].end(), emit_map[t[0]]);
      if (ec != std::errc{}) {
        is.setstate(std::ios_base::badbit);
        return is;
      }
    }
  }

  return is;
}

auto operator>>(bistream &is, HMModel &model) -> bistream & {
  if (not(is >> model.pstart_)) {
    return is;
  }
  for (size_t i = 0; i < HMModel::NSTATE; ++i) {
    if (not(is >> model.ptrans_[i])) {
      return is;
    }
  }
  for (size_t i = 0; i < HMModel::NSTATE; ++i) {
    if (not(is >> model.pemits_[i])) {
      return is;
    }
  }
  return is;
}

auto operator<<(bostream &os, const HMModel &model) -> bostream & {
  os << model.pstart_;
  for (size_t i = 0; i < HMModel::NSTATE; ++i) {
    os << model.ptrans_[i];
  }
  for (size_t i = 0; i < HMModel::NSTATE; ++i) {
    os << model.pemits_[i];
  }
  return os;
}

}  // namespace ccjieba
