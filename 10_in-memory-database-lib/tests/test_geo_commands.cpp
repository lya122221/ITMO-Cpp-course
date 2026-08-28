#include <gtest/gtest.h>
#include <memory>
#include <vector>
#include <string>
#include <list>
#include "geo_commands.hpp"
#include "general_commands.hpp"
#include "storage.hpp"

using namespace kvdb;

class GeoCommandsTest : public ::testing::Test {
protected:
  void SetUp() override {
    storage = std::make_shared<Storage>();
  }
  std::shared_ptr<Storage> storage;
};

TEST_F(GeoCommandsTest, GeoaddAndGget) {
  GeoaddCmd geoadd_cmd;
  GgetCmd gget_cmd;
  auto res_add = geoadd_cmd.Execute(storage, {
    "sicily", 
    "13.361389", "38.115556", "Palermo", 
    "15.087269", "37.502669", "Catania"
  });
  ASSERT_TRUE(std::holds_alternative<int>(res_add));
  EXPECT_EQ(std::get<int>(res_add), 2);
  auto res_gget = gget_cmd.Execute(storage, {"sicily"});
  ASSERT_TRUE(std::holds_alternative<GeoIndex>(res_gget));
  const GeoIndex& geo = std::get<GeoIndex>(res_gget);
  auto p1 = geo.GetPoint("Palermo");
  auto p2 = geo.GetPoint("Catania");
  EXPECT_TRUE(p1.has_value());
  EXPECT_TRUE(p2.has_value());
  EXPECT_DOUBLE_EQ(p1->first, 13.361389);
}

TEST_F(GeoCommandsTest, GeodistCalculation) {
  GeoaddCmd geoadd_cmd;
  GeodistCmd geodist_cmd;
  geoadd_cmd.Execute(storage, {
    "sicily", 
    "13.361389", "38.115556", "Palermo", 
    "15.087269", "37.502669", "Catania"
  });
  auto res_dist_m = geodist_cmd.Execute(storage, {"sicily", "Palermo", "Catania"});
  ASSERT_TRUE(std::holds_alternative<std::string>(res_dist_m));
  auto res_dist_km = geodist_cmd.Execute(storage, {"sicily", "Palermo", "Catania", "km"});
  ASSERT_TRUE(std::holds_alternative<std::string>(res_dist_km));
  double dist_km = std::stod(std::get<std::string>(res_dist_km));
  EXPECT_GT(dist_km, 160.0);
  EXPECT_LT(dist_km, 175.0);
  auto res_not_found = geodist_cmd.Execute(storage, {"sicily", "Palermo", "Rome"});
  EXPECT_TRUE(std::holds_alternative<std::monostate>(res_not_found));
}

TEST_F(GeoCommandsTest, GeosearchRadius) {
  GeoaddCmd geoadd_cmd;
  GeosearchCmd geosearch_cmd;
  geoadd_cmd.Execute(storage, {
    "sicily", 
    "13.361389", "38.115556", "Palermo", 
    "15.087269", "37.502669", "Catania"
  });
  auto res_search = geosearch_cmd.Execute(storage, {
    "sicily", "FROMLONLAT", "15.0", "37.0", "BYRADIUS", "200", "km", "ASC"
  });
  ASSERT_TRUE(std::holds_alternative<std::vector<std::string>>(res_search));
  auto results = std::get<std::vector<std::string>>(res_search);
  EXPECT_EQ(results.size(), 2);
  EXPECT_EQ(results[0], "Catania");
  EXPECT_EQ(results[1], "Palermo");
}

TEST_F(GeoCommandsTest, Geosearchstore) {
  GeoaddCmd geoadd_cmd;
  GeosearchstoreCmd store_cmd;
  geoadd_cmd.Execute(storage, {
    "sicily", 
    "13.361389", "38.115556", "Palermo", 
    "15.087269", "37.502669", "Catania"
  });
  auto res_store = store_cmd.Execute(storage, {
    "result_list", "sicily", "FROMLONLAT", "15.0", "37.0", "BYRADIUS", "200", "km", "ASC", "COUNT", "1"
  });
  ASSERT_TRUE(std::holds_alternative<int>(res_store));
  EXPECT_EQ(std::get<int>(res_store), 1);
  auto list_res = storage->Get("result_list");
  ASSERT_TRUE(list_res.has_value());
  auto ptr = std::get_if<std::list<std::string>>(&list_res.value());
  ASSERT_NE(ptr, nullptr);
  EXPECT_EQ(ptr->front(), "Catania");
}

TEST_F(GeoCommandsTest, ErrorsAndWrongTypes) {
  GeoaddCmd geoadd_cmd;
  SetCmd set_cmd;
  auto res_bad_coords = geoadd_cmd.Execute(storage, {
    "bad_geo", "abc", "def", "City"
  });
  EXPECT_TRUE(std::holds_alternative<std::monostate>(res_bad_coords));
  set_cmd.Execute(storage, {"str_key", "just_a_string"});
  auto res_wrong_type = geoadd_cmd.Execute(storage, {
    "str_key", "13.0", "38.0", "City"
  });
  EXPECT_TRUE(std::holds_alternative<std::monostate>(res_wrong_type));
}