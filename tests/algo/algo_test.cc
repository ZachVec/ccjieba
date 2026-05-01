#include <gtest/gtest.h>

#include <fstream>
#include <memory>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>

#include "algo/full.hh"
#include "algo/hmm.hh"
#include "algo/mix.hh"
#include "algo/mp.hh"
#include "algo/query.hh"
#include "algo/viterbi.hh"
#include "index/hmmodel.hh"
#include "index/trie.hh"

using std::operator""sv;

namespace ccjieba {

template <typename CharT, typename Traits = std::char_traits<CharT>, typename Alloc = std::allocator<CharT>>
static auto operator|(const std::vector<std::basic_string_view<CharT, Traits>> &vec,
                      std::basic_string_view<CharT, Traits> sep) -> std::basic_string<CharT, Traits, Alloc> {
  std::basic_ostringstream<CharT, Traits, Alloc> oss;
  if (vec.empty()) {
    return std::basic_string<CharT, Traits>();
  }
  auto it = vec.begin();
  oss << *it;
  for (++it; it != vec.end(); ++it) {
    oss << sep << *it;
  }
  return oss.str();
}

TEST(AlgoTest, FullSegmentTest) {
  Trie trie;
  ASSERT_FALSE((std::ifstream(DATA_ROOT "/jieba.dict.utf8") >> trie).bad());
  FullSegment segment(trie, 500);
  EXPECT_EQ(U"我/来到/北京/清华/清华大学/华大/大学", segment(U"我来到北京清华大学"sv) | U"/"sv);
  EXPECT_EQ(U"他/来到/了/网易/杭/研/大厦", segment(U"他来到了网易杭研大厦"sv) | U"/"sv);
  EXPECT_EQ(U"我/熟悉/C++/龘/龘", segment(U"我熟悉C++龘龘"sv) | U"/"sv);
}

TEST(AlgoTest, HMMSegmentTest) {
  HMModel hmm;
  ASSERT_FALSE((std::ifstream(DATA_ROOT "/hmm_model.utf8") >> hmm).bad());
  HMMSegment segment(hmm);
  EXPECT_EQ(U"我来/到/北京/清华大学", segment(U"我来到北京清华大学"sv) | U"/"sv);
  EXPECT_EQ(U"他来/到/了/网易/杭/研大厦", segment(U"他来到了网易杭研大厦"sv) | U"/"sv);
  EXPECT_EQ(U"我/熟悉/C/+/+/龘/龘", segment(U"我熟悉C++龘龘"sv) | U"/"sv);
}

TEST(AlgoTest, ViterbiTest) {
  HMModel hmm;
  ASSERT_FALSE((std::ifstream(DATA_ROOT "/hmm_model.utf8") >> hmm).bad());
  Viterbi<HMModel> viterbi(hmm);
  EXPECT_EQ(U"我来/到/北京/清华大学", viterbi(U"我来到北京清华大学"sv) | U"/"sv);
  EXPECT_EQ(U"他来/到/了/网易/杭/研大厦", viterbi(U"他来到了网易杭研大厦"sv) | U"/"sv);
  EXPECT_EQ(U"我/熟悉/C/+/+/龘/龘", viterbi(U"我熟悉C++龘龘"sv) | U"/"sv);
}

TEST(AlgoTest, MPSegmentTest) {
  Trie trie;
  ASSERT_FALSE((std::ifstream(DATA_ROOT "/jieba.dict.utf8") >> trie).bad());
  MPSegment segment(trie, 500);
  EXPECT_EQ(U"我/来到/北京/清华大学", segment(U"我来到北京清华大学"sv) | U"/"sv);
  EXPECT_EQ(U"他/来到/了/网易/杭/研/大厦", segment(U"他来到了网易杭研大厦"sv) | U"/"sv);
  EXPECT_EQ(U"我/熟悉/C++/龘/龘", segment(U"我熟悉C++龘龘"sv) | U"/"sv);
}

TEST(AlgoTest, MixSegmentTest) {
  HMModel hmm;
  ASSERT_FALSE((std::ifstream(DATA_ROOT "/hmm_model.utf8") >> hmm).bad());
  Trie trie;
  ASSERT_FALSE((std::ifstream(DATA_ROOT "/jieba.dict.utf8") >> trie).bad());
  MixSegment segment(hmm, trie, 500);
  EXPECT_EQ(U"我/来到/北京/清华大学", segment(U"我来到北京清华大学"sv) | U"/"sv);
  EXPECT_EQ(U"他/来到/了/网易/杭研/大厦", segment(U"他来到了网易杭研大厦"sv) | U"/"sv);
  EXPECT_EQ(U"我/熟悉/C++/龘/龘", segment(U"我熟悉C++龘龘"sv) | U"/"sv);
  EXPECT_EQ(U"习总书记/来/了", segment(U"习总书记来了"sv) | U"/"sv);
}

TEST(AlgoTest, QuerySegmentTest) {
  HMModel hmm;
  ASSERT_FALSE((std::ifstream(DATA_ROOT "/hmm_model.utf8") >> hmm).bad());
  Trie trie;
  ASSERT_FALSE((std::ifstream(DATA_ROOT "/jieba.dict.utf8") >> trie).bad());
  QuerySegment segment(hmm, trie, 500, {2, 3});
  EXPECT_EQ(U"我/来到/北京/清华/清华大学/华大/大学", segment(U"我来到北京清华大学"sv) | U"/"sv);
  EXPECT_EQ(U"他/来到/了/网易/杭研/大厦", segment(U"他来到了网易杭研大厦"sv) | U"/"sv);
  EXPECT_EQ(U"我/熟悉/C++/龘/龘", segment(U"我熟悉C++龘龘"sv) | U"/"sv);
}

TEST(AlgoTest, EmptyStringTest) {
  HMModel hmm;
  Trie trie;
  ASSERT_FALSE((std::ifstream(DATA_ROOT "/hmm_model.utf8") >> hmm).bad());
  ASSERT_FALSE((std::ifstream(DATA_ROOT "/jieba.dict.utf8") >> trie).bad());

  EXPECT_TRUE(MPSegment(trie, 500)(U""sv).empty());
  EXPECT_TRUE(HMMSegment(hmm)(U""sv).empty());
  EXPECT_TRUE(MixSegment(hmm, trie, 500)(U""sv).empty());
  EXPECT_TRUE(FullSegment(trie, 500)(U""sv).empty());
  EXPECT_TRUE(QuerySegment(hmm, trie, 500)(U""sv).empty());
  EXPECT_TRUE(Viterbi<HMModel>(hmm)(U""sv).empty());
}

TEST(AlgoTest, SingleCharTest) {
  HMModel hmm;
  Trie trie;
  ASSERT_FALSE((std::ifstream(DATA_ROOT "/hmm_model.utf8") >> hmm).bad());
  ASSERT_FALSE((std::ifstream(DATA_ROOT "/jieba.dict.utf8") >> trie).bad());

  // Single Chinese char — must return exactly that char
  auto mp_result = MPSegment(trie, 500)(U"我"sv);
  EXPECT_EQ(mp_result.size(), 1);
  EXPECT_EQ(mp_result[0], U"我"sv);

  auto hmm_result = HMMSegment(hmm)(U"我"sv);
  EXPECT_EQ(hmm_result.size(), 1);
  EXPECT_EQ(hmm_result[0], U"我"sv);

  auto mix_result = MixSegment(hmm, trie, 500)(U"我"sv);
  EXPECT_EQ(mix_result.size(), 1);
  EXPECT_EQ(mix_result[0], U"我"sv);
}

TEST(AlgoTest, AllASCIITest) {
  HMModel hmm;
  Trie trie;
  ASSERT_FALSE((std::ifstream(DATA_ROOT "/hmm_model.utf8") >> hmm).bad());
  ASSERT_FALSE((std::ifstream(DATA_ROOT "/jieba.dict.utf8") >> trie).bad());

  // HMM should pass through ASCII characters as-is
  auto result = HMMSegment(hmm)(U"hello"sv);
  EXPECT_EQ(result.size(), 1);
  EXPECT_EQ(result[0], U"hello"sv);
}

TEST(AlgoTest, AlphanumericTest) {
  HMModel hmm;
  ASSERT_FALSE((std::ifstream(DATA_ROOT "/hmm_model.utf8") >> hmm).bad());
  HMMSegment segment(hmm);

  // Digit-prefixed alphanumeric: "3D" stays together (Python jieba alignment)
  EXPECT_EQ(U"3D/打印", segment(U"3D打印"sv) | U"/"sv);

  // Letter-prefixed alphanumeric with decimal: "v1.2" stays together
  EXPECT_EQ(U"v1.2/版本", segment(U"v1.2版本"sv) | U"/"sv);

  // Pure decimal: "3.14" stays together
  EXPECT_EQ(U"3.14/是/圆/周率", segment(U"3.14是圆周率"sv) | U"/"sv);

  // Letter-prefixed: "A4" stays together
  EXPECT_EQ(U"A4/纸", segment(U"A4纸"sv) | U"/"sv);
}

TEST(AlgoTest, HMMSingleAsciiTest) {
  HMModel hmm;
  ASSERT_FALSE((std::ifstream(DATA_ROOT "/hmm_model.utf8") >> hmm).bad());

  // Single ASCII letter — HMM passes through directly
  auto result = HMMSegment(hmm)(U"A"sv);
  EXPECT_EQ(result.size(), 1);
  EXPECT_EQ(result[0], U"A"sv);
}

TEST(AlgoTest, MPSegmentMaxWordLenTest) {
  Trie trie;
  ASSERT_FALSE((std::ifstream(DATA_ROOT "/jieba.dict.utf8") >> trie).bad());

  // Default max_len — long matches allowed
  EXPECT_EQ(U"南京市/长江大桥", MPSegment(trie, 500)(U"南京市长江大桥"sv) | U"/"sv);

  // max_len=3 — words truncated to 3 chars, "长江大桥"(4 chars) splits into "长江"/"大桥"
  EXPECT_EQ(U"南京市/长江/大桥", MPSegment(trie, 3)(U"南京市长江大桥"sv) | U"/"sv);

  // max_len=0 — each character is a separate token
  EXPECT_EQ(U"南/京/市/长/江/大/桥", MPSegment(trie, 0)(U"南京市长江大桥"sv) | U"/"sv);
}

TEST(AlgoTest, FullSegmentNullEdgeTest) {
  Trie trie;
  ASSERT_FALSE((std::ifstream(DATA_ROOT "/jieba.dict.utf8") >> trie).bad());

  // Regression: intermediate trie nodes with null Info should not skip characters
  EXPECT_EQ(U"崎岖/的/牙齿", FullSegment(trie, 500)(U"崎岖的牙齿"sv) | U"/"sv);
}

TEST(AlgoTest, UnicodeEmojiTest) {
  Trie trie;
  HMModel hmm;
  ASSERT_FALSE((std::ifstream(DATA_ROOT "/jieba.dict.utf8") >> trie).bad());
  ASSERT_FALSE((std::ifstream(DATA_ROOT "/hmm_model.utf8") >> hmm).bad());

  // Emoji U+1F64B is a single character > 0x80, handled normally by all segmenters
  auto u32 = U"天气很好，🙋 我们去郊游。"sv;
  EXPECT_EQ(U"天气/很好/，/🙋/ /我们/去/郊游/。", MixSegment(hmm, trie, 500)(u32) | U"/"sv);
}

TEST(AlgoTest, LongInputStressTest) {
  Trie trie;
  HMModel hmm;
  ASSERT_FALSE((std::ifstream(DATA_ROOT "/jieba.dict.utf8") >> trie).bad());
  ASSERT_FALSE((std::ifstream(DATA_ROOT "/hmm_model.utf8") >> hmm).bad());

  // Build a string > 3000 UTF-8 bytes by repeating a Chinese phrase
  std::u32string u32;
  while (u32.size() < 3000) {
    u32 += U"我来到北京清华大学"sv;
  }

  {
    auto result = MixSegment(hmm, trie, 500)(u32);
    EXPECT_GT(result.size(), 0);
  }
  {
    auto result = FullSegment(trie, 500)(u32);
    EXPECT_GT(result.size(), 0);
  }
  {
    auto result = MPSegment(trie, 500)(u32);
    EXPECT_GT(result.size(), 0);
  }
  {
    auto result = HMMSegment(hmm)(u32);
    EXPECT_GT(result.size(), 0);
  }
  {
    auto result = QuerySegment(hmm, trie, 500)(u32);
    EXPECT_GT(result.size(), 0);
  }
}

}  // namespace ccjieba
