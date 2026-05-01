#include "algo/hmm.hh"

#include <cctype>
#include <cstddef>
#include <string_view>
#include <vector>

#include "index/hmmodel.hh"

namespace ccjieba {

HMMSegment::HMMSegment(const HMModel &model) : viterbi_(model) {}

auto HMMSegment::operator()(std::u32string_view str) const -> std::vector<std::u32string_view> {
  size_t u = 0;
  size_t v = 0;
  const size_t n = str.size();
  std::vector<std::u32string_view> segs;
  while (v < n) {
    if (str[v] >= 0x80) {
      ++v;
      continue;
    }
    if (u != v) {
      auto internal = viterbi_(str.substr(u, v - u));
      segs.insert(segs.end(), internal.begin(), internal.end());
    }
    u = v;
    v = v + 1;
    if (std::isalpha(static_cast<int>(str[u]))) {
      while (v < n and std::isalnum(static_cast<int>(str[v]))) {
        ++v;
      }
    } else if (std::isdigit(static_cast<int>(str[u]))) {
      while (v < n and (std::isdigit(static_cast<int>(str[v])) or str[v] == U'.')) {
        ++v;
      }
    }
    segs.push_back(str.substr(u, v - u));
    u = v;
  }
  if (v != u) {
    auto internal = viterbi_(str.substr(u, v - u));
    segs.insert(segs.end(), internal.begin(), internal.end());
  }
  return segs;
}

}  // namespace ccjieba
