#include <gtest/gtest.h>

#include <sstream>
#include <string_view>

#include "index/hmmodel.hh"

using std::operator""sv;

namespace ccjieba {

TEST(HMModelTest, Utf8LoadTest) {
  HMModel model;
  std::istringstream iss(
      // pstart: 4
      "-0.26268660809250016 -3.14e+100 -3.14e+100 -1.4652633398537678\n"
      // ptrans: 4x4
      "-3.14e+100 -0.510825623765990 -0.916290731874155 -3.14e+100\n"
      "-0.5897149736854513 -3.14e+100 -3.14e+100 -0.8085250474669937\n"
      "-3.14e+100 -0.33344856811948514 -1.2603623820268226 -3.14e+100\n"
      "-0.7211965654669841 -3.14e+100 -3.14e+100 -0.6658631448798212\n"
      // pemits: B E M S
      "耀:-10.460283,蘄:-11.015514\n"
      "耀:-9.266706,蘄:-17.334482\n"
      "耀:-8.476517,蘄:-14.372296\n"
      "耀:-11.216223,蘄:-10.005820");
  EXPECT_TRUE(iss >> model);
  auto ctx = model(U"耀"sv);

  EXPECT_DOUBLE_EQ(ctx.pstart(0), -0.26268660809250016);
  EXPECT_DOUBLE_EQ(ctx.pstart(1), -3.14e+100);
  EXPECT_DOUBLE_EQ(ctx.pstart(2), -3.14e+100);
  EXPECT_DOUBLE_EQ(ctx.pstart(3), -1.4652633398537678);

  EXPECT_DOUBLE_EQ(ctx.ptrans(0, 0), -3.14e+100);
  EXPECT_DOUBLE_EQ(ctx.ptrans(0, 1), -0.510825623765990);
  EXPECT_DOUBLE_EQ(ctx.ptrans(0, 2), -0.916290731874155);
  EXPECT_DOUBLE_EQ(ctx.ptrans(0, 3), -3.14e+100);

  EXPECT_DOUBLE_EQ(ctx.ptrans(1, 0), -0.5897149736854513);
  EXPECT_DOUBLE_EQ(ctx.ptrans(1, 1), -3.14e+100);
  EXPECT_DOUBLE_EQ(ctx.ptrans(1, 2), -3.14e+100);
  EXPECT_DOUBLE_EQ(ctx.ptrans(1, 3), -0.8085250474669937);

  EXPECT_DOUBLE_EQ(ctx.ptrans(2, 0), -3.14e+100);
  EXPECT_DOUBLE_EQ(ctx.ptrans(2, 1), -0.33344856811948514);
  EXPECT_DOUBLE_EQ(ctx.ptrans(2, 2), -1.2603623820268226);
  EXPECT_DOUBLE_EQ(ctx.ptrans(2, 3), -3.14e+100);

  EXPECT_DOUBLE_EQ(ctx.ptrans(3, 0), -0.7211965654669841);
  EXPECT_DOUBLE_EQ(ctx.ptrans(3, 1), -3.14e+100);
  EXPECT_DOUBLE_EQ(ctx.ptrans(3, 2), -3.14e+100);
  EXPECT_DOUBLE_EQ(ctx.ptrans(3, 3), -0.6658631448798212);

  EXPECT_DOUBLE_EQ(ctx.pemits(0, 0), -10.460283);
  EXPECT_DOUBLE_EQ(ctx.pemits(1, 0), -9.266706);
  EXPECT_DOUBLE_EQ(ctx.pemits(2, 0), -8.476517);
  EXPECT_DOUBLE_EQ(ctx.pemits(3, 0), -11.216223);
}

TEST(HMModelTest, BinaryRoundTripTest) {
  HMModel original;
  {
    std::istringstream iss(
        "-0.26268660809250016 -3.14e+100 -3.14e+100 -1.4652633398537678\n"
        "-3.14e+100 -0.510825623765990 -0.916290731874155 -3.14e+100\n"
        "-0.5897149736854513 -3.14e+100 -3.14e+100 -0.8085250474669937\n"
        "-3.14e+100 -0.33344856811948514 -1.2603623820268226 -3.14e+100\n"
        "-0.7211965654669841 -3.14e+100 -3.14e+100 -0.6658631448798212\n"
        "耀:-10.460283,蘄:-11.015514\n"
        "耀:-9.266706,蘄:-17.334482\n"
        "耀:-8.476517,蘄:-14.372296\n"
        "耀:-11.216223,蘄:-10.005820");
    ASSERT_TRUE(iss >> original);
  }

  bostream os;
  os << original;

  HMModel restored;
  bistream is(os.data(), os.size());
  ASSERT_TRUE(is >> restored);

  // Compare via Context
  auto orig_ctx = original(U"耀"sv);
  auto rest_ctx = restored(U"耀"sv);

  for (size_t s = 0; s < 4; ++s) {
    EXPECT_DOUBLE_EQ(rest_ctx.pstart(s), orig_ctx.pstart(s));
    EXPECT_DOUBLE_EQ(rest_ctx.pemits(s, 0), orig_ctx.pemits(s, 0));
    for (size_t t = 0; t < 4; ++t) {
      EXPECT_DOUBLE_EQ(rest_ctx.ptrans(s, t), orig_ctx.ptrans(s, t));
    }
  }
}

}  // namespace ccjieba
