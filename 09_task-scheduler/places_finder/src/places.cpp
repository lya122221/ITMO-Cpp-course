#include <iostream>
#include <string>
#include <cpr/cpr.h>
#include <nlohmann/json.hpp>
using json = nlohmann::json;
#include "model.h"

json GetPlaces(const Location& location, const Weather& weather, int radius_m) {
  std::string filter_str = "circle:" + std::to_string(location.lon) + "," + 
                                       std::to_string(location.lat) + "," + 
                                       std::to_string(radius_m);

  std::string bias_str = "proximity:" + std::to_string(location.lon) + "," + std::to_string(location.lat);

  std::string categories_str = weather.is_bad ? "entertainment.museum,entertainment.cinema,catering.cafe,commercial.shopping_mall"
                                              : "leisure.park,tourism.sights,catering.restaurant";

  cpr::Response response = cpr::Get(
    cpr::Url{"https://api.geoapify.com/v2/places"},
    cpr::Proxies{{"http", ""}, {"https", ""}},
    cpr::Parameters{
      {"categories", categories_str}, 
      {"filter", filter_str},
      {"bias", bias_str},
      {"limit", "10"},
      {"apiKey", GEOAPIFY_API_KEY}
    }
  );

  if (response.status_code != 200) {
    throw std::runtime_error("Error geoapify API");
  }

  json j = json::parse(response.text);

  return j;
}

std::vector<Place> ParsePlaces(json j) {
  std::vector<Place> places_list;

  if (!j.contains("features") || !j["features"].is_array()) {
    return places_list;
  }

  for (const auto& feature : j["features"]) {
    if (!feature.contains("properties")) continue;
    
    auto props = feature["properties"];
    Place current_place;

    current_place.name = props.value("name", "Без названия");

    current_place.address = props.value("address_line2", "Нет адреса");

    int dist_meters = props.value("distance", 0);
    current_place.distance = dist_meters;

    if (props.contains("categories") && props["categories"].is_array() && !props["categories"].empty()) {
      current_place.category = props["categories"][0].get<std::string>();
    } else {
      current_place.category = "Неизвестно";
    }

    if (current_place.name != "Без названия") {
      places_list.push_back(current_place);
    }
  }

  return places_list;
}