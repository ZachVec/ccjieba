/// @file jieba.cc
/// @brief Jieba facade implementation: template instantiation, segmentation, extraction, and tagging pipelines.

#include "jieba.hh"

#include <cassert>
#include <cstddef>
#include <optional>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>
#include <vector>

#include "algo/extractor.hh"
#include "algo/full.hh"
#include "algo/hmm.hh"
#include "algo/mix.hh"
#include "algo/mp.hh"
#include "algo/query.hh"
#include "algo/tagger.hh"
#include "index/trie.hh"
#include "keyword.hh"
#include "utils/pre_segment.hh"
#include "utils/str_utils.hh"

namespace ccjieba {

/// @brief Dispatch cut() to the correct segmenter based on Algo type.
///
/// Pre-segments the input on delimiters (space, tab, comma, Chinese full stop),
/// segments each piece, and converts results back to UTF-8.
template <typename Algo>
auto Jieba::cut(std::string_view str, std::optional<size_t> max_word_length) const -> std::vector<std::string> {
  std::u32string sentence;
  if (not utf8_to_utf32(str, sentence)) {
    return {};
  }
  size_t word_len = max_word_length.value_or(500);
  std::optional<Algo> segmenter;
  if constexpr (std::is_same_v<Algo, MPSegment> or std::is_same_v<Algo, FullSegment>) {
    segmenter.emplace(trie_, word_len);
  } else if constexpr (std::is_same_v<Algo, HMMSegment>) {
    segmenter.emplace(hmm_);
  } else if constexpr (std::is_same_v<Algo, MixSegment> or std::is_same_v<Algo, QuerySegment>) {
    segmenter.emplace(hmm_, trie_, word_len);
  } else {
    static_assert(sizeof(Algo) == 0, "Unsupported algorithm");
  }

  std::vector<std::u32string_view> segments;
  segments.reserve(sentence.size());
  for (const std::u32string_view &piece : PreSegmenter(U" \t，。")(sentence)) {
    auto segs = (*segmenter)(piece);
    segments.insert(segments.end(), segs.begin(), segs.end());
  }

  size_t size = segments.size();
  std::vector<std::string> ret(segments.size());
  for (size_t i = 0; i < size; ++i) {
    assert(utf32_to_utf8(segments[i], ret[i]));
  }
  segments.shrink_to_fit();
  return ret;
}

/// @brief Extract top-N keywords via MixSegment + KeywordExtractor.
auto Jieba::extract(std::string_view str, size_t top_n) const -> std::vector<Keyword> {
  return KeywordExtractor(idf_, stop_words_)(cut<MixSegment>(str), top_n);
}

/// @brief Tag a single word: first try dictionary lookup, then heuristic classification.
auto Jieba::tag_word(std::string_view str) const -> std::string_view {
  std::u32string encoded;
  if (not utf8_to_utf32(str, encoded)) {
    return Tagger::TAG_UNK;
  }
  auto ret = trie_.match(encoded);
  if (ret != nullptr) {
    return ret->tag;
  }
  return Tagger::classify_unknown_word(encoded);
}

/// @brief Segment and tag an entire sentence via Tagger.
auto Jieba::tag_sentence(std::string_view str) const -> std::vector<std::pair<std::string, std::string_view>> {
  std::u32string sentence;
  if (not utf8_to_utf32(str, sentence)) {
    return {};
  }
  Tagger tagger(hmm_, trie_, 500);
  std::vector<std::pair<std::string, std::string_view>> tags;
  tags.reserve(sentence.size());
  std::string u8word;

  for (const std::u32string_view &piece : PreSegmenter(U" \t，。")(sentence)) {
    for (const auto &[word, tag] : tagger(piece)) {
      assert(utf32_to_utf8(word, u8word));
      tags.emplace_back(std::move(u8word), tag);
    }
  }
  tags.shrink_to_fit();
  return tags;
}

// Explicit template instantiation for all supported segmenter types.
template std::vector<std::string> Jieba::cut<MPSegment>(std::string_view, std::optional<size_t>) const;
template std::vector<std::string> Jieba::cut<FullSegment>(std::string_view, std::optional<size_t>) const;
template std::vector<std::string> Jieba::cut<HMMSegment>(std::string_view, std::optional<size_t>) const;
template std::vector<std::string> Jieba::cut<MixSegment>(std::string_view, std::optional<size_t>) const;
template std::vector<std::string> Jieba::cut<QuerySegment>(std::string_view, std::optional<size_t>) const;

}  // namespace ccjieba
