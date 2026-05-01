#include "index/trie.hh"

#include <gtest/gtest.h>

#include <cmath>
#include <sstream>
#include <string_view>

#include "utils/dag.hh"

using std::operator""sv;

namespace ccjieba {

TEST(TrieTest, LoadTest1) {
  Trie trie;
  EXPECT_FALSE((std::istringstream("汉堡 2 n\n书包 3 n\n") >> trie).bad());
}

TEST(TrieTest, LoadTest2) {
  Trie trie;
  EXPECT_TRUE((std::istringstream("汉堡 2 n\n书包 n\n") >> trie).bad());  // error format on second line
}

TEST(TrieTest, IteratorTest1) {
  Trie trie;
  ASSERT_TRUE(not(std::istringstream("汉堡 2 n\n书包 3 n\n") >> trie).bad());
  const auto &[s, info] = *trie.begin();
  EXPECT_EQ(s, U"书包");
  EXPECT_DOUBLE_EQ(info->weight, std::log(3.0 / 5.0));
  EXPECT_EQ(info->tag, "n");
}

TEST(TrieTest, IteratorTest2) {
  Trie trie;
  ASSERT_TRUE(not(std::istringstream("汉堡 2 n\n书包 3 n\n") >> trie).bad());
  std::vector<std::pair<std::u32string, const Info *>> entries(trie.begin(), trie.end());
  ASSERT_EQ(entries.size(), 2ULL);
  EXPECT_EQ(entries[0].first, U"书包");
  EXPECT_EQ(entries[0].second->weight, std::log(3.0 / 5.0));
  EXPECT_EQ(entries[0].second->tag, "n");
  EXPECT_EQ(entries[1].first, U"汉堡");
  EXPECT_EQ(entries[1].second->weight, std::log(2.0 / 5.0));
  EXPECT_EQ(entries[1].second->tag, "n");
}

TEST(TrieTest, IteratorTest3) {
  Trie trie;
  ASSERT_TRUE(not(std::istringstream("书包 3 n\n书架 2 n\n书 1 n\n") >> trie).bad());
  std::vector<std::pair<std::u32string, const Info *>> entries(trie.begin(), trie.end());
  ASSERT_EQ(entries.size(), 3ULL);
  EXPECT_EQ(entries[0].first, U"书架");
  EXPECT_EQ(entries[0].second->weight, std::log(2.0 / 6.0));
  EXPECT_EQ(entries[0].second->tag, "n");
  EXPECT_EQ(entries[1].first, U"书包");
  EXPECT_EQ(entries[1].second->weight, std::log(3.0 / 6.0));
  EXPECT_EQ(entries[1].second->tag, "n");
  EXPECT_EQ(entries[2].first, U"书");
  EXPECT_EQ(entries[2].second->weight, std::log(1.0 / 6.0));
  EXPECT_EQ(entries[2].second->tag, "n");
}

TEST(TrieTest, MatchTest) {
  Trie trie;
  ASSERT_FALSE((std::istringstream("书 2 n\n书架 3 n\n书包 3 n\n") >> trie).bad());
  auto weight = [total = 8.0](size_t freq) -> double {
    return std::log(static_cast<double>(freq) / total);
  };

  auto info = trie.match(U"书包");
  EXPECT_NE(info, nullptr);
  EXPECT_DOUBLE_EQ(info->weight, weight(3));
  EXPECT_EQ(info->tag, "n");

  info = trie.match(U"书架");
  EXPECT_NE(info, nullptr);
  EXPECT_DOUBLE_EQ(info->weight, weight(3));
  EXPECT_EQ(info->tag, "n");

  info = trie.match(U"书");
  EXPECT_NE(info, nullptr);
  EXPECT_DOUBLE_EQ(info->weight, weight(2));
  EXPECT_EQ(info->tag, "n");

  EXPECT_EQ(trie.match(U""), nullptr);
  EXPECT_EQ(trie.match(U"课本"), nullptr);
}

TEST(TrieTest, SearchTest) {
  Trie trie;
  ASSERT_FALSE((std::istringstream("清华 3 n\n清华大学 5 n\n华大 2 n\n大 3 adj\n") >> trie).bad());
  auto weight = [total = 13.0](size_t freq) -> double {
    return std::log(static_cast<double>(freq) / total);
  };

  const auto &graph = trie.search(U"清华大学"sv, 500);
  EXPECT_EQ(graph.size(), 4);

  EXPECT_EQ(graph[0].size(), 3);
  EXPECT_EQ(graph[0][0].first, 0);  // 清->清: null
  EXPECT_EQ(graph[0][0].second, nullptr);
  EXPECT_EQ(graph[0][1].first, 1);  // 清->华: 3 n
  EXPECT_EQ(graph[0][1].second->weight, weight(3));
  EXPECT_EQ(graph[0][2].first, 3);  // 清->学: 5 n
  EXPECT_EQ(graph[0][2].second->weight, weight(5));

  EXPECT_EQ(graph[1].size(), 2);
  EXPECT_EQ(graph[1][0].first, 1);  // 华->华: null
  EXPECT_EQ(graph[1][0].second, nullptr);
  EXPECT_EQ(graph[1][1].first, 2);  // 华->大: 2 n
  EXPECT_EQ(graph[1][1].second->weight, weight(2));

  EXPECT_EQ(graph[2].size(), 1);
  EXPECT_EQ(graph[2][0].first, 2);  // 大->大: 3 adj
  EXPECT_EQ(graph[2][0].second->weight, weight(3));
  EXPECT_EQ(graph[2][0].second->tag, "adj");
  EXPECT_EQ(graph[3].size(), 1);  // 学->学: null
  EXPECT_EQ(graph[3][0].first, 3);
  EXPECT_EQ(graph[3][0].second, nullptr);
}

TEST(TrieTest, UserTrieLoadTest) {
  Trie trie;
  ASSERT_FALSE((std::istringstream("汉堡 2 n\n书包 3 n\n") >> trie).bad());
  // Before user dict: "云计算" is not in trie
  EXPECT_EQ(trie.match(U"云计算"), nullptr);

  ASSERT_FALSE((std::istringstream("云计算 5 n\n") >> trie.user()).bad());
  // Weight uses original total_frequence_ (5), not updated total (10)
  EXPECT_DOUBLE_EQ(trie.match(U"云计算")->weight, std::log(5.0 / 5.0));
  EXPECT_EQ(trie.match(U"云计算")->tag, "n");
}

TEST(TrieTest, UserTrieLoadNoFreqTest) {
  Trie trie;
  ASSERT_FALSE((std::istringstream("汉堡 2 n\n书包 3 n\n") >> trie).bad());
  ASSERT_FALSE((std::istringstream("云计算 n\n") >> trie.user()).bad());
  EXPECT_NE(trie.match(U"云计算"), nullptr);
  EXPECT_EQ(trie.match(U"云计算")->tag, "n");
  // Without freq specified, weight falls back to minimum_weight_
  EXPECT_DOUBLE_EQ(trie.match(U"云计算")->weight, trie.minimum_weight());
}

TEST(TrieTest, UserTrieInUserDictTest) {
  Trie trie;
  ASSERT_FALSE((std::istringstream("汉堡 2 n\n") >> trie).bad());
  EXPECT_FALSE(trie.in_userdict(U'云'));
  ASSERT_FALSE((std::istringstream("云计算 5 n\n") >> trie.user()).bad());
  EXPECT_TRUE(trie.in_userdict(U'云'));
  EXPECT_FALSE(trie.in_userdict(U'汉'));
}

TEST(TrieTest, BinaryRoundTripTest) {
  Trie original;
  ASSERT_FALSE((std::istringstream("汉堡 2 n\n书包 3 n\n书 1 n\n") >> original).bad());

  bostream os;
  os << original;

  Trie restored;
  bistream is(os.data(), os.size());
  is >> restored;

  EXPECT_DOUBLE_EQ(restored.minimum_weight(), original.minimum_weight());
  EXPECT_DOUBLE_EQ(restored.maximum_weight(), original.maximum_weight());
  EXPECT_EQ(restored.max_word_length(), original.max_word_length());

  auto weight = original.match(U"汉堡");
  auto weight2 = restored.match(U"汉堡");
  ASSERT_NE(weight, nullptr);
  ASSERT_NE(weight2, nullptr);
  EXPECT_DOUBLE_EQ(weight->weight, weight2->weight);
  EXPECT_EQ(weight->tag, weight2->tag);

  EXPECT_EQ(restored.match(U"课本"), nullptr);

  // Verify iteration returns same entries
  std::vector<std::u32string> orig_keys, rest_keys;
  for (auto it = original.begin(); it != original.end(); ++it) {
    orig_keys.push_back((*it).first);
  }
  for (auto it = restored.begin(); it != restored.end(); ++it) {
    rest_keys.push_back((*it).first);
  }
  EXPECT_EQ(orig_keys.size(), rest_keys.size());
}

}  // namespace ccjieba
