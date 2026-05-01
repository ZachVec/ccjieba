#include "utils/io.hh"

#include <gtest/gtest.h>

namespace ccjieba {

TEST(IOTest, SerializeTest) {
  bostream os;
  os << 1 << "hello" << 3;
  const std::byte *data = os.data();
  const size_t size = os.size();
  ASSERT_EQ(size, sizeof(int) + sizeof(size_t) + sizeof("hello") + sizeof(int) - 1);
  EXPECT_EQ(*reinterpret_cast<const int *>(data), 1);
  data += sizeof(int);
  EXPECT_EQ(*reinterpret_cast<const size_t *>(data), sizeof("hello") - 1);
  data += sizeof(size_t);
  EXPECT_EQ(std::string(reinterpret_cast<const char *>(data), sizeof("hello") - 1), "hello");
  data += sizeof("hello") - 1;
  EXPECT_EQ(*reinterpret_cast<const int *>(data), 3);
}

TEST(IOTest, DeserializeTest) {
  bostream os;
  os << 1 << "hello" << 3;
  bistream is(os.data(), os.size());
  int val1;
  std::string val2;
  int val3;
  ASSERT_TRUE(is >> val1 >> val2 >> val3);
  EXPECT_EQ(val1, 1);
  EXPECT_EQ(val2, "hello");
  EXPECT_EQ(val3, 3);
}

}  // namespace ccjieba
