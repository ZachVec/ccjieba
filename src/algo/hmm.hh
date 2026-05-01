#pragma once

#include <string_view>
#include <vector>

#include "algo/viterbi.hh"
#include "index/hmmodel.hh"

namespace ccjieba {

class HMMSegment {
  Viterbi<HMModel> viterbi_;

 public:
  HMMSegment() = delete;
  explicit HMMSegment(const HMModel &model);

  auto operator()(std::u32string_view str) const -> std::vector<std::u32string_view>;
};

}  // namespace ccjieba
