#include "api_client.h"
#include "model.h"
#include <cpr/cpr.h>
#include <nlohmann/json.hpp>
using json = nlohmann::json;
#include "serialization.h"
#include <iostream>
#include <optional>
#include <unordered_map>
#include <filesystem>
#include <fstream>
#include <stdexcept>

APIClient::APIClient(const std::string& api_key) : api_key_(api_key), cities_path_(std::filesystem::temp_directory_path() / "cities_cache.json") {
  LoadCitiesFile();
}

std::optional<Data> APIClient::GetDataFromRequest(const Request& request) {
  cpr::Response api_response = cpr::Get(
    cpr::Url{"https://api.rasp.yandex-net.ru/v3.0/search"},
    cpr::Parameters{
      {"apikey", api_key_},
      {"from", request.from_code},
      {"to", request.to_code},
      {"date", request.date},
      {"transfers", "true"}
    }
  );

  if (api_response.status_code != 200) {
    std::cerr << "API error: " << api_response.status_code << " " << api_response.text << std::endl;
    return std::nullopt;
  }

  json json_response = json::parse(api_response.text);
  Data data = json_response.get<Data>();
  data.routes = ValidateRoutes(data.routes);
  return data;
}

std::vector<Route> APIClient::ValidateRoutes(const std::vector<Route>& routes) {
  std::vector<Route> validated_routes;
  for (auto it = routes.begin(); it != routes.end(); it++) {
    if ((it->segments).size() < 3) {
      validated_routes.push_back(*it);
    }
  }

  return validated_routes;
}

std::string APIClient::GetCityCode(const std::string& city) {
  std::string city_code = cities_[city];
  if (city_code.empty()) {
    throw std::runtime_error("City not found");
  } 
  return city_code;
}

json APIClient::GetCitiesFromApi() {
  cpr::Response api_response = cpr::Get(
    cpr::Url{"https://api.rasp.yandex-net.ru/v3.0/stations_list"},
    cpr::Parameters{
      {"apikey", api_key_},
      {"lang", "ru_RU"}
    }
  );

  if (api_response.status_code != 200) {
    std::cerr << "API error: " << api_response.status_code << " " << api_response.text << std::endl;
    throw;
  }

  json json_response = json::parse(api_response.text);
  return json_response;
}

std::unordered_map<std::string, std::string> APIClient::ParseCities(const json& cities) {
  std::unordered_map<std::string, std::string> result;

  if (cities.contains("countries") && cities["countries"].is_array()) {
    for (const auto& country : cities["countries"]) {
      if (country.contains("regions") && country["regions"].is_array()) {
        for (const auto& region : country["regions"]) {
          if (region.contains("settlements") && region["settlements"].is_array()) {
            for (const auto& settlement : region["settlements"]) {
              std::string city_name = settlement.value("title", "");
              if (city_name.empty()) {
                continue;
              }

              if (settlement.contains("codes") && settlement["codes"].is_object()) {
                std::string city_code = settlement["codes"].value("yandex_code", "");
                if (!city_code.empty()) {
                  result[city_name] = city_code;
                }
              }
            }
          }
        }
      }
    }
  }
  return result;
}

void APIClient::CreateFileForCitiesAndParse()  {
  std::ofstream new_file(cities_path_);
  if (!new_file) {
    throw std::runtime_error("Cannot create cities cache file");
  }
  json cities_json = GetCitiesFromApi();
  cities_ = ParseCities(cities_json);
  
  cities_json = cities_;
  new_file << cities_json.dump(0);
}

void APIClient::LoadCitiesFile() {
  std::ifstream file(cities_path_);
  if (!file) {
    CreateFileForCitiesAndParse();
    return;
  }

  try {
    json cities_json = json::parse(file);
    cities_ = cities_json.get<std::unordered_map<std::string, std::string>>();
  } catch (const json::exception& e){
    std::cerr << "Cities cache load error: " << e.what() << std::endl;

    CreateFileForCitiesAndParse();
    return;
  }
}

APIClient::APIClient(const std::string& api_key, const std::string& cities_path) : api_key_(api_key), cities_path_(cities_path) {
    LoadCitiesFile();
}