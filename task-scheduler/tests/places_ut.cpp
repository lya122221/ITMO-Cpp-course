#include <gtest/gtest.h>
#include <nlohmann/json.hpp>
#include <vector>
#include "places.h"
#include "model.h"
using json = nlohmann::json;

TEST(PlacesTest, ParseValidPlaces) {
  json data = R"({
    "features": [
      {
        "properties": {
          "name": "Эрмитаж",
          "address_line2": "Дворцовая пл., 2",
          "distance": 150,
          "categories": ["tourism.museum"]
        }
      },
      {
        "properties": {
          "name": "Кофе Хауз",
          "address_line2": "Невский пр., 10",
          "distance": 500,
          "categories": ["catering.cafe"]
        }
      }
    ]
  })"_json;

  std::vector<Place> places = ParsePlaces(data);

  ASSERT_EQ(places.size(), 2);
  
  EXPECT_EQ(places[0].name, "Эрмитаж");
  EXPECT_EQ(places[0].distance, 150);
  EXPECT_EQ(places[0].category, "tourism.museum");

  EXPECT_EQ(places[1].name, "Кофе Хауз");
  EXPECT_EQ(places[1].distance, 500);
}

TEST(PlacesTest, ParseEmptyOrInvalidJson) {
  json empty_json = R"({})"_json;
  std::vector<Place> places = ParsePlaces(empty_json);
  EXPECT_TRUE(places.empty());

  json no_features = R"({"type": "FeatureCollection"})"_json;
  places = ParsePlaces(no_features);
  EXPECT_TRUE(places.empty());
}

TEST(PlacesTest, SkipPlacesWithoutName) {
  json data = R"({
    "features": [
      {
        "properties": {
          "distance": 10
        }
      }
    ]
  })"_json;

  std::vector<Place> places = ParsePlaces(data);

  EXPECT_TRUE(places.empty());
}

TEST(PlacesTest, DefaultValuesForMissingFields) {
  json data = R"({
    "features": [
      {
        "properties": {
          "name": "Скрытое место",
          "distance": 42
        }
      }
    ]
  })"_json;

  std::vector<Place> places = ParsePlaces(data);

  ASSERT_EQ(places.size(), 1);
  
  EXPECT_EQ(places[0].name, "Скрытое место");
  EXPECT_EQ(places[0].address, "Нет адреса");
  EXPECT_EQ(places[0].category, "Неизвестно");
  EXPECT_EQ(places[0].distance, 42);
}

TEST(PlacesTest, SkipFeaturesWithoutProperties) {
  json data = R"({
    "features": [
      { "type": "Feature" }, 
      { "properties": { "name": "Нормальное место", "distance": 100 } }
    ]
  })"_json;

  std::vector<Place> places = ParsePlaces(data);

  ASSERT_EQ(places.size(), 1);
  EXPECT_EQ(places[0].name, "Нормальное место");
}