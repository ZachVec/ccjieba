#pragma once

#include <array>
#include <cstddef>
#include <istream>
#include <string_view>
#include <unordered_map>

#include "utils/io.hh"

namespace ccjieba {

template <typename T>
class Viterbi;

class HMModel {
  friend auto operator>>(std::istream &is, HMModel &model) -> std::istream &;
  friend auto operator>>(bistream &is, HMModel &model) -> bistream &;
  friend auto operator<<(bostream &os, const HMModel &model) -> bostream &;
  friend class Viterbi<HMModel>;
  constexpr static double LOWEST = -3.14e+100;  // INF
  constexpr static size_t DSTATE = 1;           // default state: E
  constexpr static size_t NSTATE = 4;           // total # of states: 4, B E M S

  class Context {
    friend class HMModel;

    std::u32string_view str_;
    const HMModel &model_;
    explicit Context(std::u32string_view str, const HMModel &model) : str_(str), model_(model) {}

   public:
    inline auto pstart(size_t state) const -> double { return model_.pstart_[state]; }

    inline auto ptrans(size_t prev, size_t curr) const -> double { return model_.ptrans_[prev][curr]; }

    inline auto pemits(size_t state, size_t index) const -> double {
      const auto &map = model_.pemits_[state];
      auto it = map.find(str_[index]);
      return it == map.end() ? LOWEST : it->second;
    }

    static inline auto is_end(size_t state) -> bool { return state % 2 == 1; }
  };

 public:
  HMModel() = default;

  inline auto operator()(std::u32string_view str) const -> Context { return Context(str, *this); }

 private:
  std::array<double, NSTATE> pstart_;
  std::array<std::array<double, NSTATE>, NSTATE> ptrans_;
  std::array<std::unordered_map<char32_t, double>, NSTATE> pemits_;
};

}  // namespace ccjieba
