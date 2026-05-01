#pragma once

#include <algorithm>
#include <cstddef>
#include <limits>
#include <string_view>
#include <type_traits>
#include <utility>
#include <vector>

namespace ccjieba {

template <typename ModelT>
class Viterbi {
  using mcontext = decltype(std::declval<const ModelT>()(std::declval<std::u32string_view>()));
  using pstart_t = decltype(std::declval<const mcontext>().pstart(std::declval<size_t>()));
  using ptrans_t = decltype(std::declval<const mcontext>().ptrans(std::declval<size_t>(), std::declval<size_t>()));
  using pemits_t = decltype(std::declval<const mcontext>().ptrans(std::declval<size_t>(), std::declval<size_t>()));
  using is_end_t = decltype(std::declval<const mcontext>().is_end(std::declval<size_t>()));
  static_assert(std::is_same_v<pstart_t, double>, "[Signature] double pstart(size_t) const;");
  static_assert(std::is_same_v<ptrans_t, double>, "[Signature] double ptrans(size_t, size_t) const;");
  static_assert(std::is_same_v<pemits_t, double>, "[Signature] double pemits(size_t, size_t) const;");
  static_assert(std::is_same_v<is_end_t, bool>, "[Signature] bool is_end(size_t) const;");

  static_assert(std::is_floating_point_v<decltype(ModelT::LOWEST)> and ModelT::LOWEST < 0);
  static_assert(std::is_integral_v<decltype(ModelT::NSTATE)> and std::is_integral_v<decltype(ModelT::DSTATE)>);
  static_assert(0 <= ModelT::DSTATE and ModelT::DSTATE < ModelT::NSTATE);

  constexpr static size_t NSTATE = ModelT::NSTATE;
  constexpr static size_t DSTATE = ModelT::DSTATE;
  constexpr static double LOWEST = ModelT::LOWEST;
  const ModelT &model_;

 public:
  Viterbi() = delete;
  explicit Viterbi(const ModelT &model) : model_(model) {}

  auto operator()(std::u32string_view str) const -> std::vector<std::u32string_view> {
    if (str.empty()) {
      return {};
    }

    auto model = model_(str);
    std::vector<double> probs(2 * NSTATE, LOWEST);
    std::vector<size_t> paths(str.size() * NSTATE, DSTATE);

    for (size_t state = 0; state < NSTATE; ++state) {
      probs[state] = model.pstart(state) + model.pemits(state, 0ull);
    }

    double *prev_probs = probs.data();
    double *curr_probs = probs.data() + NSTATE;
    size_t *curr_paths = paths.data() + NSTATE;
    for (size_t i = 1; i < str.size(); ++i) {
      for (size_t state = 0; state < NSTATE; ++state) {
        double &curr_prob = (curr_probs[state] = LOWEST);
        size_t &curr_path = curr_paths[state];
        double pemit = model.pemits(state, i);
        for (size_t prev_state = 0; prev_state < NSTATE; ++prev_state) {
          double prob = prev_probs[prev_state] + model.ptrans(prev_state, state) + pemit;
          if (prob > curr_prob) {
            curr_prob = prob;
            curr_path = prev_state;
          }
        }
      }
      std::swap(prev_probs, curr_probs);
      curr_paths += NSTATE;
    }

    std::vector<size_t> path(str.size());
    double final_prob = std::numeric_limits<double>::lowest();
    for (size_t state = 0; state < NSTATE; ++state) {
      if (mcontext::is_end(state) and prev_probs[state] > final_prob) {
        final_prob = prev_probs[state];
        path.back() = state;
      }
    }

    const size_t *prev_paths = paths.data() + paths.size() - NSTATE;
    for (ptrdiff_t i = static_cast<ptrdiff_t>(str.size()) - 2; i >= 0; --i) {
      path[i] = prev_paths[path[i + 1]];
      prev_paths -= NSTATE;
    }

    std::vector<std::u32string_view> ret;
    ret.reserve(std::count_if(path.begin(), path.end(), mcontext::is_end));
    for (size_t u = 0, v = 0; v < str.size(); ++v) {
      if (mcontext::is_end(path[v])) {
        ret.push_back(str.substr(u, v + 1 - u));
        u = v + 1;
      }
    }
    return ret;
  }
};

}  // namespace ccjieba
