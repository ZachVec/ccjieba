#include "algo/mix.hh"

#include <cstddef>
#include <string_view>
#include <vector>

#include "algo/hmm.hh"
#include "algo/internal.hh"
#include "index/trie.hh"

namespace ccjieba {

MixSegment::MixSegment(const HMModel &hmm, const Trie &trie, size_t max_word_length)
  : hmm_(hmm), trie_(trie), max_word_length_(max_word_length) {}

auto MixSegment::operator()(std::u32string_view str) const -> std::vector<std::u32string_view> {
  auto graph = trie_.search(str, max_word_length_);
  auto segments = mp(graph, trie_.minimum_weight());
  return mix(str, segments, hmm_, trie_);
}

auto mix(std::u32string_view str, const Segments &segs, const HMModel &model, const Trie &trie)
    -> std::vector<std::u32string_view> {
  const HMMSegment hmm(model);
  std::vector<std::u32string_view> ret;
  ret.reserve(str.size());
  for (size_t u = 0, v = 0; u < segs.size(); u = v) {
    v = std::get<0>(segs[u]) + 1;
    if (v > u + 1 || trie.in_userdict(str[u])) {
      ret.push_back(str.substr(u, v - u));
      continue;
    }
    for (; v < segs.size(); ++v) {
      if (v != std::get<0>(segs[v]) || trie.in_userdict(str[v])) {
        break;
      }
    }
    auto subsegs = hmm(str.substr(u, v - u));
    ret.insert(ret.end(), subsegs.begin(), subsegs.end());
  }
  ret.shrink_to_fit();
  return ret;
}

}  // namespace ccjieba
