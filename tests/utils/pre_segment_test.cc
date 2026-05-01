#include "utils/pre_segment.hh"

#include <gtest/gtest.h>

#include <sstream>
#include <string>
#include <string_view>

using std::operator""sv;

namespace ccjieba {

static auto join(const PreSegment &segment, std::u32string_view sep) -> std::u32string {
  auto begin = segment.begin();
  auto end = segment.end();
  if (begin == end) {
    return U"";
  }
  std::basic_ostringstream<char32_t> oss;
  oss << *begin;
  while (++begin != end) {
    oss << sep << *begin;
  }
  return oss.str();
}

class PreSegmenterTest : public testing::TestWithParam<std::pair<std::u32string_view, std::u32string_view>> {};

TEST_P(PreSegmenterTest, SimpleTest) {
  PreSegmenter seg(U"，。！；");
  const auto &[original, expected] = GetParam();
  EXPECT_EQ(join(seg(original), U"/"), expected);
}

INSTANTIATE_TEST_SUITE_P(
    , PreSegmenterTest,
    testing::ValuesIn(std::vector<std::pair<std::u32string_view, std::u32string_view>>{
        // clang-format off
        {U"你好世界"sv, U"你好世界"sv},
        {U"你好，美丽的，世界"sv, U"你好/，/美丽的/，/世界"sv},
        {U"。。你好，美丽的，世界"sv, U"。/。/你好/，/美丽的/，/世界"sv},
        {
          U"我是蓝翔技工拖拉机学院手扶拖拉机专业的。不用多久，我就会升职加薪，当上总经理，出任CEO，迎娶白富美，走上人生巅峰。"sv, 
          U"我是蓝翔技工拖拉机学院手扶拖拉机专业的/。/不用多久/，/我就会升职加薪/，/当上总经理/，/出任CEO/，/迎娶白富美/，/走上人生巅峰/。"sv},
        {U"我来自北京邮电大学。。。学号123456，用AK47"sv, U"我来自北京邮电大学/。/。/。/学号123456/，/用AK47"sv},
        // clang-format on
    }));

}  // namespace ccjieba
