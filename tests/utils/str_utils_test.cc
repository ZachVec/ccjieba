#include "utils/str_utils.hh"

#include <gtest/gtest.h>

#include <string>
#include <string_view>
#include <vector>

using std::operator""sv;

namespace ccjieba {

TEST(StrUtilsTest, SplitTest) {
  std::vector<std::string_view> expected{"a"sv, "b"sv, "c"sv, "d"sv};
  EXPECT_EQ(split(std::string_view("a b c d"), " "sv), expected);
}

TEST(StrUtilsTest, StripTest) {
  EXPECT_EQ(strip("aaabbbbaaa"sv, "a"sv), "bbbb");
  EXPECT_EQ(strip("aaa bbbb aaa"sv, "a "sv), "bbbb");
  EXPECT_EQ(strip("\f\v  \thello world \n"sv, "\f\v \t\n"sv), "hello world"sv);
  EXPECT_EQ(strip(U"😄你好，世界！😄"sv, U"😄"sv), U"你好，世界！"sv);
  EXPECT_EQ(strip(U"🙈🙉🙈"sv, U"🙈"sv), U"🙉"sv);
}

TEST(StrUtilsTest, Utf8ToUtf32) {
  std::u32string u32;
  EXPECT_TRUE(utf8_to_utf32("你好，世界！"sv, u32));
  EXPECT_EQ(u32, U"你好，世界！");
}

TEST(StrUtilsTest, Utf32ToUtf8) {
  std::string u8;
  EXPECT_TRUE(utf32_to_utf8(U"你好，世界！"sv, u8));
  EXPECT_EQ(u8, "你好，世界！");
}

}  // namespace ccjieba
