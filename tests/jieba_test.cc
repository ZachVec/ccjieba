#include "jieba.hh"

#include <gtest/gtest.h>

#include <fstream>
#include <ostream>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>

#include "algo/full.hh"
#include "algo/hmm.hh"
#include "algo/mix.hh"
#include "algo/mp.hh"
#include "algo/query.hh"

using std::operator""sv;

namespace ccjieba {

static auto operator|(const std::vector<std::string> &vec, std::string_view sep) -> std::string {
  std::ostringstream oss;
  if (vec.empty()) {
    return "";
  }
  auto it = vec.begin();
  oss << *it;
  for (++it; it != vec.end(); ++it) {
    oss << sep << *it;
  }
  return oss.str();
}

template <typename T>
static auto operator<<(std::ostream &os, const std::vector<T> &data) -> std::ostream &;

static auto operator<<(std::ostream &os, const Keyword &keyword) -> std::ostream & {
  os << R"({"word": ")" << keyword.word << '\"';
  os << R"(, "offset": )" << keyword.offsets;
  os << R"(, "weight": )" << keyword.weight;
  return os << R"(})";
}

static auto operator<<(std::ostream &os, const std::pair<std::string, std::string_view> &tag) -> std::ostream & {
  return os << '\"' << tag.first << ':' << tag.second << '\"';
}

template <typename T>
static auto operator<<(std::ostream &os, const std::vector<T> &data) -> std::ostream & {
  if (data.empty()) {
    return os << "[]";
  }
  os << '[' << data.front();
  for (size_t i = 1; i < data.size(); ++i) {
    os << ", " << data[i];
  }
  return os << ']';
}

class JiebaTest : public testing::Test {
 protected:
  void SetUp() override {
    ASSERT_FALSE((std::ifstream(DATA_ROOT "/jieba.dict.utf8") >> jieba.trie_).bad());
    ASSERT_FALSE((std::ifstream(DATA_ROOT "/hmm_model.utf8") >> jieba.hmm_).bad());
    ASSERT_FALSE((std::ifstream(DATA_ROOT "/idf.utf8") >> jieba.idf_).bad());
    ASSERT_FALSE((std::ifstream(DATA_ROOT "/stop_words.utf8") >> jieba.stop_words_).bad());
  }

  template <typename Algo>
  auto segment(std::string_view str) const -> std::string {
    return jieba.cut<Algo>(str) | "/"sv;
  }

  auto extract(std::string_view str, size_t top_n) const -> std::string {
    std::ostringstream oss;
    oss << jieba.extract(str, top_n);
    return oss.str();
  }

  auto tag(std::string_view str) const -> std::string {
    std::ostringstream oss;
    oss << jieba.tag_sentence(str);
    return oss.str();
  }

  Jieba jieba;
};

TEST_F(JiebaTest, SegmentTest) {
  // clang-format off
  EXPECT_EQ(segment<FullSegment>("我来到北京清华大学"), "我/来到/北京/清华/清华大学/华大/大学");
  EXPECT_EQ(segment<FullSegment>("他来到了网易杭研大厦"), "他/来到/了/网易/杭/研/大厦");
  EXPECT_EQ(segment<FullSegment>("我熟悉C++龘龘"), "我/熟悉/C++/龘/龘");
  EXPECT_EQ(segment<FullSegment>("小明硕士毕业于中国科学院计算所，后在日本京都大学深造"), "小/明/硕士/毕业/于/中国/中国科学院/科学/科学院/学院/计算/计算所/，/后/在/日本/日本京都大学/京都/京都大学/大学/深造");

  EXPECT_EQ(segment<HMMSegment>("我来到北京清华大学"), "我来/到/北京/清华大学");
  EXPECT_EQ(segment<HMMSegment>("他来到了网易杭研大厦"), "他来/到/了/网易/杭/研大厦");
  EXPECT_EQ(segment<HMMSegment>("我熟悉C++龘龘"), "我/熟悉/C/+/+/龘/龘");
  EXPECT_EQ(segment<HMMSegment>("小明硕士毕业于中国科学院计算所，后在日本京都大学深造"), "小明/硕士/毕业于/中国/科学院/计算/所/，/后/在/日/本京/都/大学/深造");

  EXPECT_EQ(segment<MPSegment>("我来到北京清华大学"), "我/来到/北京/清华大学");
  EXPECT_EQ(segment<MPSegment>("他来到了网易杭研大厦"), "他/来到/了/网易/杭/研/大厦");
  EXPECT_EQ(segment<MPSegment>("我熟悉C++龘龘"), "我/熟悉/C++/龘/龘");
  EXPECT_EQ(segment<MPSegment>("小明硕士毕业于中国科学院计算所，后在日本京都大学深造"), "小/明/硕士/毕业/于/中国科学院/计算所/，/后/在/日本京都大学/深造");

  EXPECT_EQ(segment<MixSegment>("我来到北京清华大学"), "我/来到/北京/清华大学");
  EXPECT_EQ(segment<MixSegment>("他来到了网易杭研大厦"), "他/来到/了/网易/杭研/大厦");
  EXPECT_EQ(segment<MixSegment>("我熟悉C++龘龘"), "我/熟悉/C++/龘/龘");
  EXPECT_EQ(segment<MixSegment>("小明硕士毕业于中国科学院计算所，后在日本京都大学深造"), "小明/硕士/毕业/于/中国科学院/计算所/，/后/在/日本京都大学/深造");
  EXPECT_EQ(segment<MixSegment>("我是拖拉机学院手扶拖拉机专业的。不用多久，我就会升职加薪，当上CEO，走上人生巅峰。"), "我/是/拖拉机/学院/手扶拖拉机/专业/的/。/不用/多久/，/我/就/会/升职/加薪/，/当上/CEO/，/走上/人生/巅峰/。");

  EXPECT_EQ(segment<QuerySegment>("我来到北京清华大学"), "我/来到/北京/清华/清华大学/华大/大学");
  EXPECT_EQ(segment<QuerySegment>("他来到了网易杭研大厦"), "他/来到/了/网易/杭研/大厦");
  EXPECT_EQ(segment<QuerySegment>("我熟悉C++龘龘"), "我/熟悉/C++/龘/龘");
  EXPECT_EQ(segment<QuerySegment>("小明硕士毕业于中国科学院计算所，后在日本京都大学深造"), "小明/硕士/毕业/于/中国/中国科学院/科学/科学院/学院/计算/计算所/，/后/在/日本/日本京都大学/京都/大学/深造");
  // clang-format on
}

TEST_F(JiebaTest, UserDictTest) {
  EXPECT_EQ(segment<MixSegment>("令狐冲是云计算行业的专家"), "令狐冲/是/云/计算/行业/的/专家");
  ASSERT_FALSE((std::ifstream(TEST_DATA_ROOT "/user.dict.utf8") >> jieba.trie_.user()).bad());
  EXPECT_EQ(segment<MixSegment>("令狐冲是云计算行业的专家"), "令狐冲/是/云计算/行业/的/专家");
}

TEST_F(JiebaTest, KeywordExtractTest) {
  EXPECT_EQ(extract("我是拖拉机学院手扶拖拉机专业的。不用多久，我就会升职加薪，当上CEO，走上人生巅峰。"sv, 5),
            R"([{"word": "CEO", "offset": [93], "weight": 11.7392},)"sv
            R"( {"word": "升职", "offset": [72], "weight": 10.8562},)"sv
            R"( {"word": "加薪", "offset": [78], "weight": 10.6426},)"sv
            R"( {"word": "手扶拖拉机", "offset": [21], "weight": 10.0089},)"sv
            R"( {"word": "巅峰", "offset": [111], "weight": 9.49396}])"sv);
}

TEST_F(JiebaTest, TagTest) {
  ASSERT_FALSE((std::ifstream(TEST_DATA_ROOT "/user.dict.utf8") >> jieba.trie_.user()).bad());
  EXPECT_EQ(
      tag("我是蓝翔技工拖拉机学院手扶拖拉机专业的。不用多久，我就会升职加薪，当上总经理，出任CEO，迎娶白富美，走上人生巅峰。"sv),
      R"(["我:r", "是:v", "蓝翔:nz", "技工:n", "拖拉机:n", "学院:n", "手扶拖拉机:n", "专业:n", "的:uj", "。:x", )"sv
      R"("不用:v", "多久:m", "，:x", "我:r", "就:d", "会:v", "升职:v", "加薪:nr", "，:x", "当上:t", "总经理:n", "，:x", )"sv
      R"("出任:v", "CEO:eng", "，:x", "迎娶:v", "白富:x", "美:ns", "，:x", "走上:v", "人生:n", "巅峰:n", "。:x"])");
}

TEST_F(JiebaTest, TagWordTest) {
  ASSERT_FALSE((std::ifstream(TEST_DATA_ROOT "/user.dict.utf8") >> jieba.trie_.user()).bad());

  // Known dictionary word gets trie tag
  EXPECT_EQ(jieba.tag_word("手扶拖拉机"sv), "n");

  // English word detection
  EXPECT_EQ(jieba.tag_word("CEO"sv), "eng");

  // Number detection
  EXPECT_EQ(jieba.tag_word("123"sv), "m");
  EXPECT_EQ(jieba.tag_word("3.14"sv), "eng");  // '.' makes it not purely numeric

  // Unknown Chinese word
  EXPECT_EQ(jieba.tag_word("龘龘"sv), "x");
}

}  // namespace ccjieba
