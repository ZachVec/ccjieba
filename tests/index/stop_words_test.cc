#include "index/stop_words.hh"

#include <gtest/gtest.h>

#include <sstream>
#include <string>
#include <string_view>

#include "utils/io.hh"

using std::operator""sv;

namespace ccjieba {

TEST(StopWordsTest, LoadAndLookupTest) {
  StopWordsDictionary sw;
  EXPECT_FALSE((std::istringstream("the\nis\nat\n") >> sw).bad());

  EXPECT_TRUE(sw["the"sv]);
  EXPECT_TRUE(sw["is"sv]);
  EXPECT_TRUE(sw["at"sv]);
  EXPECT_FALSE(sw["hello"sv]);
  EXPECT_FALSE(sw[""sv]);
}

TEST(StopWordsTest, LoadEmptyTest) {
  StopWordsDictionary sw;
  EXPECT_FALSE((std::istringstream("") >> sw).bad());
  EXPECT_FALSE(sw["anything"sv]);
}

TEST(StopWordsTest, BinaryRoundTripTest) {
  StopWordsDictionary original;
  ASSERT_FALSE((std::istringstream("the\nis\nat\n") >> original).bad());

  bostream os;
  os << original;

  StopWordsDictionary restored;
  bistream is(os.data(), os.size());
  is >> restored;

  EXPECT_TRUE(restored["the"sv]);
  EXPECT_TRUE(restored["is"sv]);
  EXPECT_TRUE(restored["at"sv]);
  EXPECT_FALSE(restored["hello"sv]);
}

}  // namespace ccjieba
