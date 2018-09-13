#include "cybertron/time/duration.h"
#include <iostream>
#include "cybertron/time/time.h"
#include "gtest/gtest.h"

namespace apollo {
namespace cybertron {

TEST(DurationTest, constructor) {
  Duration duration(100);
  EXPECT_EQ(100, duration.ToNanosecond());

  duration = Duration(1.1);
  EXPECT_EQ(1100000000UL, duration.ToNanosecond());
  EXPECT_DOUBLE_EQ(1.1, duration.ToSecond());

  duration = Duration(1, 1);
  EXPECT_EQ(1000000001UL, duration.ToNanosecond());
  EXPECT_DOUBLE_EQ(1.000000001, duration.ToSecond());

  Duration d2(duration);
  EXPECT_TRUE(duration == d2);
}

TEST(DurationTest, operators) {
  Duration d1(100);
  Duration d2(200);
  Duration d3(300);
  EXPECT_TRUE(d1 != d3);
  EXPECT_TRUE(d1 < d3);
  EXPECT_TRUE(d1 <= d3);
  EXPECT_TRUE(d3 > d1);
  EXPECT_TRUE(d3 >= d1);
  EXPECT_TRUE(d1 + d2 == d3);
  EXPECT_TRUE(d2 == d1 * 2);
  EXPECT_TRUE(d3 - d2 == d1);
  EXPECT_TRUE((d1 += d2) == d3);
  EXPECT_TRUE(d1 >= d3);
  EXPECT_TRUE(d1 <= d3);
  EXPECT_TRUE(Duration(100) == (d1 -= d2));
  EXPECT_TRUE(d2 == (d1 *= 2));

  std::cout << "Duration is: " << d1 << std::endl;
}

TEST(DurationTest, is_zero) {
  Duration duration;
  EXPECT_TRUE(duration.IsZero());
}

}  // namespace cybertron
}  // namespace apollo

int main(int argc, char** argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}