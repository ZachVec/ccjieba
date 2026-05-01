#include <array>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <jieba.hh>
#include <keyword.hh>
#include <ostream>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace {

using std::operator""sv;

auto operator<<(std::ostream &os, const ccjieba::Keyword &keyword) -> std::ostream & {
  return os << '\"' << keyword.word << ':' << keyword.weight << '\"';
}

auto operator<<(std::ostream &os, const std::pair<std::string, std::string_view> &tag) -> std::ostream & {
  return os << '\"' << tag.first << ':' << tag.second << '\"';
}

struct title {  // NOLINT(readability-identifier-naming)
  std::string_view str;
  int length;
  friend auto operator<<(std::ostream &os, const title &title) -> std::ostream & {
    return os << '[' << std::right << std::setw(title.length) << title.str << "] ";
  }
};

template <typename T>
struct join {  // NOLINT(readability-identifier-naming)
  std::string_view leading_chars_;
  std::string_view ending_chars_;
  std::string_view sep_;
  const std::vector<T> values_;

  join(std::string_view leading_chars, std::string_view ending_chars, std::string_view sep, std::vector<T> values)
    : leading_chars_(leading_chars), ending_chars_(ending_chars), sep_(sep), values_(std::move(values)) {}

  friend auto operator<<(std::ostream &os, const join<T> &obj) -> std::ostream & {
    os << obj.leading_chars_;
    if (not obj.values_.empty()) {
      os << obj.values_.front();
      for (size_t i = 1; i < obj.values_.size(); ++i) {
        os << obj.sep_ << obj.values_[i];
      }
    }
    return os << obj.ending_chars_;
  }
};

template <typename T>
auto operator|(std::vector<T> values, std::array<std::string_view, 3> chars) -> join<T> {
  return join<T>(chars[0], chars[1], chars[2], std::move(values));
}

}  // namespace

auto main() -> int {
  ccjieba::Jieba jieba;
  if ((std::ifstream(DATA_ROOT "/jieba.dict.utf8") >> jieba.trie_).bad()) {
    std::cout << "Failed to read jieba.dict.utf8" << std::endl;
    return 1;
  }
  if ((std::ifstream(DATA_ROOT "/hmm_model.utf8") >> jieba.hmm_).bad()) {
    std::cout << "Failed to read hmm_model.utf8" << std::endl;
    return 1;
  }
  if ((std::ifstream(DATA_ROOT "/idf.utf8") >> jieba.idf_).bad()) {
    std::cout << "Failed to read idf.utf8" << std::endl;
    return 1;
  }
  if ((std::ifstream(DATA_ROOT "/stop_words.utf8") >> jieba.stop_words_).bad()) {
    std::cout << "Failed to read stop_words.utf8" << std::endl;
    return 1;
  }
  std::vector<std::string> sentences{"我来到北京清华大学",
                                     "他来到了网易杭研大厦",
                                     "我熟悉C++龘龘",
                                     "小明硕士毕业于中国科学院计算所，后在日本京都大学深造"};

  std::array<std::string_view, 3> chars{""sv, ""sv, "/"sv};
  for (const auto &sentence : sentences) {
    std::cout << title{"Orignal"sv, 20} << sentence << std::endl;
    std::cout << title{"MPSegment"sv, 20} << (jieba.cut<ccjieba::MPSegment>(sentence) | chars) << std::endl;
    std::cout << title{"HMMSegment"sv, 20} << (jieba.cut<ccjieba::HMMSegment>(sentence) | chars) << std::endl;
    std::cout << title{"MixSegment"sv, 20} << (jieba.cut<ccjieba::MixSegment>(sentence) | chars) << std::endl;
    std::cout << title{"FullSegment"sv, 20} << (jieba.cut<ccjieba::FullSegment>(sentence) | chars) << std::endl;
    std::cout << title{"QuerySegment"sv, 20} << (jieba.cut<ccjieba::QuerySegment>(sentence) | chars) << std::endl;
  }

  std::string_view s = "令狐冲是云计算行业的专家"sv;
  std::cout << title{"Before User Dict", 20} << (jieba.cut(s) | chars) << std::endl;
  if ((std::ifstream(TEST_DATA_ROOT "/user.dict.utf8") >> jieba.trie_.user()).bad()) {
    std::cout << "Failed to read user.dict.utf8" << std::endl;
    return 1;
  }
  std::cout << title{"After User Dict", 20} << (jieba.cut(s) | chars) << std::endl;

  chars = {"["sv, "]"sv, ", "sv};
  s = "我是拖拉机学院手扶拖拉机专业的。不用多久，我就会升职加薪，当上CEO，走上人生巅峰。"sv;
  std::cout << title{"Extraction", 20} << (jieba.extract(s, 5) | chars) << std::endl;

  s = "我是蓝翔技工拖拉机学院手扶拖拉机专业的。不用多久，我就会升职加薪，当上总经理，出任CEO，迎娶白富美，走上人生巅峰。"sv;
  std::cout << title{"Tagging", 20} << (jieba.tag_sentence(s) | chars) << std::endl;

  return 0;
}
