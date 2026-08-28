#include <gtest/gtest.h>
#include "model.h"

TEST(ModelTest, GetCurrentHourReturnsValidRange) {
  int hour = GetCurrentHour();
  EXPECT_GE(hour, 0);
  EXPECT_LE(hour, 23);
}