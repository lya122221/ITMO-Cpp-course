#include "model.h"
#include <cpr/cpr.h>
#include <nlohmann/json.hpp>
using json = nlohmann::json;
#include <vector>

const std::unordered_map<int, std::pair<std::string, bool>> day_weather_icons = {
  {0,  {"☀️", false}},
  {1,  {"🌤️", false}},
  {2,  {"⛅", false}},
  {3,  {"☁️", false}},
  {45, {"🌫️", true}},
  {48, {"🌫️", true}},
  {51, {"🌦️", true}},
  {53, {"🌦️", true}},
  {55, {"🌧️", true}},
  {56, {"🌧️", true}},
  {57, {"🌧️", true}},
  {61, {"🌧️", true}},
  {63, {"🌧️", true}},
  {65, {"🌧️", true}},
  {66, {"🌧️", true}},
  {67, {"🌧️", true}},
  {71, {"❄️", true}},
  {73, {"❄️", true}},
  {75, {"❄️", true}},
  {77, {"❄️", true}},
  {80, {"🌦️", true}},
  {81, {"🌦️", true}},
  {82, {"🌧️", true}},
  {85, {"🌨️", true}},
  {86, {"🌨️", true}},
  {95, {"⛈️", true}},
  {96, {"⛈️", true}},
  {99, {"⛈️", true}}
};

const std::unordered_map<int, std::pair<std::string, bool>> night_weather_icons = {
  {0,  {"🌙", false}},
  {1,  {"🌙", false}},
  {2,  {"☁️", false}},
  {3,  {"☁️", false}},
  {45, {"🌫️", true}},
  {48, {"🌫️", true}},
  {51, {"🌧️", true}},
  {53, {"🌧️", true}},
  {55, {"🌧️", true}},
  {56, {"🌧️", true}},
  {57, {"🌧️", true}},
  {61, {"🌧️", true}},
  {63, {"🌧️", true}},
  {65, {"🌧️", true}},
  {66, {"🌧️", true}},
  {67, {"🌧️", true}},
  {71, {"❄️", true}},
  {73, {"❄️", true}},
  {75, {"❄️", true}},
  {77, {"❄️", true}},
  {80, {"🌧️", true}},
  {81, {"🌧️", true}},
  {82, {"🌧️", true}},
  {85, {"🌨️", true}},
  {86, {"🌨️", true}},
  {95, {"⛈️", true}},
  {96, {"⛈️", true}},
  {99, {"⛈️", true}}
};

json GetWeather(Location location) {
  cpr::Response response = cpr::Get(
    cpr::Url{"https://api.open-meteo.com/v1/forecast"},
    cpr::Proxies{{"http", ""}, {"https", ""}},
    cpr::Parameters{
      {"latitude", std::to_string(location.lat)},
      {"longitude", std::to_string(location.lon)},
      {"hourly", "temperature_2m,apparent_temperature,weather_code"},
      {"timezone", "auto"},
      {"past_days", "0"},
      {"forecast_days", "1"}
    }
  );

  if (response.status_code != 200) {
    throw std::runtime_error("Error open meteo API");
  }

  json data = json::parse(response.text);

  return data;
}

Weather ParseWeather(json data, int hour) {
  std::vector<std::string> times = data["hourly"]["time"];
  std::vector<double> temps = data["hourly"]["temperature_2m"];
  std::vector<double> apparent_temps = data["hourly"]["apparent_temperature"];
  std::vector<int> weather_codes = data["hourly"]["weather_code"];

  Weather weather;
  bool is_day = false;
  if (hour > 4 && hour < 22) {
    is_day = true;
  }

  const auto& map_to_use = is_day ? day_weather_icons : night_weather_icons;

  int code = weather_codes[hour];

  auto it = map_to_use.find(code);
  if (it != map_to_use.end()) {
    weather.emoji = it->second.first;
    weather.is_bad = it->second.second;
  }
  weather.temp = temps[hour];
  weather.apparent_temp = apparent_temps[hour];

  return weather;
}