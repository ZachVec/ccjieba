#include <gtest/gtest.h>

#include <fstream>
#include <memory>
#include <sstream>
#include <string>
#include <string_view>

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

TEST(AlgoTest, HMMSingleAsciiTest) {
  HMModel hmm;
  ASSERT_FALSE((std::ifstream(DATA_ROOT "/hmm_model.utf8") >> hmm).bad());

  // Single ASCII letter — HMM passes through directly
  auto result = HMMSegment(hmm)(U"A"sv);
  EXPECT_EQ(result.size(), 1);
  EXPECT_EQ(result[0], U"A"sv);
}

}  // namespace ccjieba
