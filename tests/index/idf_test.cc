#include "index/idf.hh"

#include <gtest/gtest.h>

#include <sstream>
#include <string>
#include <string_view>

#include "utils/io.hh"

using std::operator""sv;

namespace ccjieba {

TEST(IDFTest, LoadAndLookupTest) {
  InverseDocumentFrequence idf;
  EXPECT_FALSE((std::istringstream("hello 1.5\nworld 2.5\n") >> idf).bad());

  EXPECT_DOUBLE_EQ(idf["hello"sv], 1.5);
  EXPECT_DOUBLE_EQ(idf["world"sv], 2.5);
  // Missing word should return average
  EXPECT_DOUBLE_EQ(idf["unknown"sv], (1.5 + 2.5) / 2.0);
}

TEST(IDFTest, LoadEmptyTest) {
  InverseDocumentFrequence idf;
  EXPECT_FALSE((std::istringstream("") >> idf).bad());
  EXPECT_DOUBLE_EQ(idf["anything"sv], 0.0);
}

TEST(IDFTest, LoadBadFormatTest) {
  InverseDocumentFrequence idf;
  EXPECT_TRUE((std::istringstream("hello 1.5 extra\n") >> idf).bad());
}

TEST(IDFTest, BinaryRoundTripTest) {
  InverseDocumentFrequence original;
  ASSERT_FALSE((std::istringstream("hello 1.5\nworld 2.5\n") >> original).bad());

  bostream os;
  os << original;

  InverseDocumentFrequence restored;
  bistream is(os.data(), os.size());
  ASSERT_TRUE(is >> restored);

  EXPECT_DOUBLE_EQ(restored["hello"sv], 1.5);
  EXPECT_DOUBLE_EQ(restored["world"sv], 2.5);
  EXPECT_DOUBLE_EQ(restored["unknown"sv], original["unknown"sv]);
}

}  // namespace ccjieba
