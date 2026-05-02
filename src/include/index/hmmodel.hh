/// @file hmmodel.hh
/// @brief Hidden Markov Model for Viterbi-based unknown-word segmentation and POS tagging.

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

/// @brief 4-state Hidden Markov Model (B/E/M/S) for the Viterbi decoder.
///
/// States: B (begin), E (end), M (middle), S (single character).
/// The is_end() test exploits that end-states (E=1, S=3) have odd indices.
/// Use operator() to obtain a Context bound to an input string for decoding.
class HMModel {
  friend auto operator>>(std::istream &is, HMModel &model) -> std::istream &;
  friend auto operator>>(bistream &is, HMModel &model) -> bistream &;
  friend auto operator<<(bostream &os, const HMModel &model) -> bostream &;
  friend class Viterbi<HMModel>;
  constexpr static double LOWEST = -3.14e+100;  ///< Sentinel for impossible transitions.
  constexpr static size_t DSTATE = 1;           ///< Default state: E (end).
  constexpr static size_t NSTATE = 4;           ///< Number of states: B, E, M, S.

  /// @brief A view bound to an input string, providing emission/transition probabilities.
  class Context {
    friend class HMModel;

    std::u32string_view str_;
    const HMModel &model_;
    explicit Context(std::u32string_view str, const HMModel &model) : str_(str), model_(model) {}

   public:
    /// @brief Initial state probability.
    inline auto pstart(size_t state) const -> double { return model_.pstart_[state]; }

    /// @brief Transition probability from @p prev to @p curr.
    inline auto ptrans(size_t prev, size_t curr) const -> double { return model_.ptrans_[prev][curr]; }

    /// @brief Emission probability of @p state at the given character index.
    inline auto pemits(size_t state, size_t index) const -> double {
      const auto &map = model_.pemits_[state];
      auto it = map.find(str_[index]);
      return it == map.end() ? LOWEST : it->second;
    }

    /// @brief True if @p state is an end state (E=1 or S=3 — odd indices).
    static inline auto is_end(size_t state) -> bool { return state % 2 == 1; }
  };

 public:
  HMModel() = default;

  /// @brief Create a decoding context bound to @p str.
  inline auto operator()(std::u32string_view str) const -> Context { return Context(str, *this); }

 private:
  std::array<double, NSTATE> pstart_;                           ///< log(initial state probabilities).
  std::array<std::array<double, NSTATE>, NSTATE> ptrans_;        ///< log(transition probabilities) [prev][curr].
  std::array<std::unordered_map<char32_t, double>, NSTATE> pemits_; ///< log(emission probabilities) per state.
};

}  // namespace ccjieba
