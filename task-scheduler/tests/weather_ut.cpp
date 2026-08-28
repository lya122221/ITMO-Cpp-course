#include <gtest/gtest.h>
#include <nlohmann/json.hpp>
#include "weather.h"
#include "model.h"
using json = nlohmann::json;

json CreateDummyWeatherJson() {
  json data;
  std::vector<std::string> times(24, "2023-01-01T00:00");
  std::vector<double> temps(24, 15.5);
  std::vector<double> apparent_temps(24, 12.0);
  std::vector<int> codes(24, 0);

  data["hourly"]["time"] = times;
  data["hourly"]["temperature_2m"] = temps;
  data["hourly"]["apparent_temperature"] = apparent_temps;
  data["hourly"]["weather_code"] = codes;
  return data;
}

TEST(WeatherTest, ParseDayTimeWeather) {
  json data = CreateDummyWeatherJson();
  data["hourly"]["weather_code"][14] = 3;
  data["hourly"]["temperature_2m"][14] = 25.0;

  Weather w = ParseWeather(data, 14);

  EXPECT_DOUBLE_EQ(w.temp, 25.0);
  EXPECT_EQ(w.emoji, "☁️");
  EXPECT_FALSE(w.is_bad);
}

TEST(WeatherTest, ParseNightTimeWeather) {
  json data = CreateDummyWeatherJson();
  data["hourly"]["weather_code"][2] = 0;
  data["hourly"]["temperature_2m"][2] = -5.0;

  Weather w = ParseWeather(data, 2);

  EXPECT_DOUBLE_EQ(w.temp, -5.0);
  EXPECT_FALSE(w.is_bad); 
}