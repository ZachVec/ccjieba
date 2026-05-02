/// @file hmm.hh
/// @brief HMM-based segmenter for unregistered (non-dictionary) character runs.

#pragma once

#include <string_view>
#include <vector>

#include "algo/viterbi.hh"
#include "index/hmmodel.hh"

namespace ccjieba {

/// @brief Segments run-on character sequences using the Viterbi algorithm over the HMM model.
///
/// Scans for ASCII/alphanumeric spans (which are emitted as single tokens) and passes
/// the remaining CJK-character runs through the Viterbi decoder.
class HMMSegment {
  Viterbi<HMModel> viterbi_;

 public:
  HMMSegment() = delete;
  explicit HMMSegment(const HMModel &model);

  /// @brief Segment a UTF-32 string.
  /// @param str Input string.
  /// @return Vector of non-overlapping u32string_views.
  auto operator()(std::u32string_view str) const -> std::vector<std::u32string_view>;
};

}  // namespace ccjieba
