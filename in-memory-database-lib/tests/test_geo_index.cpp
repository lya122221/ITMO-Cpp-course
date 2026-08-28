#include <gtest/gtest.h>
#include <cmath>
#include <string>
#include "geo_index.hpp"

using namespace kvdb;

class GeoIndexTest : public ::testing::Test {
protected:
  GeoIndex geo;
};

TEST_F(GeoIndexTest, AddAndGetPoint) {
  geo.AddPoint("Moscow", 37.6173, 55.7558);
  auto pt = geo.GetPoint("Moscow");
  ASSERT_TRUE(pt.has_value());
  EXPECT_DOUBLE_EQ(pt->first, 37.6173);
  EXPECT_DOUBLE_EQ(pt->second, 55.7558);
  auto missing = geo.GetPoint("Paris");
  EXPECT_FALSE(missing.has_value());
}

TEST_F(GeoIndexTest, OverwritePoint) {
  geo.AddPoint("PointA", 10.0, 20.0);
  geo.AddPoint("PointA", 30.0, 40.0);
  auto pt = geo.GetPoint("PointA");
  ASSERT_TRUE(pt.has_value());
  EXPECT_DOUBLE_EQ(pt->first, 30.0);
  EXPECT_DOUBLE_EQ(pt->second, 40.0);
}

TEST_F(GeoIndexTest, CalculateDistanceAccuracyAndUnits) {
  geo.AddPoint("Moscow", 37.6173, 55.7558);
  geo.AddPoint("SPB", 30.3141, 59.9386);
  double dist_km = geo.CalculateDistance("Moscow", "SPB", "km");
  EXPECT_GT(dist_km, 630.0);
  EXPECT_LT(dist_km, 640.0);
  double dist_m = geo.CalculateDistance("Moscow", "SPB", "m");
  EXPECT_GT(dist_m, 630000.0);
  EXPECT_LT(dist_m, 640000.0);
  double dist_mi = geo.CalculateDistance("Moscow", "SPB", "mi");
  EXPECT_GT(dist_mi, 390.0);
  EXPECT_LT(dist_mi, 400.0);
  double dist_ft = geo.CalculateDistance("Moscow", "SPB", "ft");
  EXPECT_GT(dist_ft, 2000000.0);
}

TEST_F(GeoIndexTest, CalculateDistanceMissingMember) {
  geo.AddPoint("Moscow", 37.6173, 55.7558);
  double dist = geo.CalculateDistance("Moscow", "London");
  EXPECT_DOUBLE_EQ(dist, -1.0);
}

TEST_F(GeoIndexTest, SearchRadiusLogic) {
  geo.AddPoint("Center", 0.0, 0.0);
  geo.AddPoint("Close", 0.0, 1.0);
  geo.AddPoint("Far", 0.0, 2.0);
  auto res_150 = geo.SearchRadius(0.0, 0.0, 150.0, "km", true);
  ASSERT_EQ(res_150.size(), 2);
  EXPECT_EQ(res_150[0], "Center");
  EXPECT_EQ(res_150[1], "Close");
  auto res_50 = geo.SearchRadius(0.0, 0.0, 50.0, "km", false);
  ASSERT_EQ(res_50.size(), 1);
  EXPECT_EQ(res_50[0], "Center");
  auto res_300 = geo.SearchRadius(0.0, 0.0, 300.0, "km", false);
  ASSERT_EQ(res_300.size(), 3);
}

TEST_F(GeoIndexTest, MemoryUsageTracking) {
  size_t base_mem = geo.GetMemoryUsage();
  EXPECT_GT(base_mem, 0);
  geo.AddPoint("PointA", 12.34, 56.78);
  size_t new_mem = geo.GetMemoryUsage();
  EXPECT_GT(new_mem, base_mem);
}