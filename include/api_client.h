#pragma once
#include <string>
#include <unordered_map>
#include <vector>
#include <optional>
#include "model.h"
#include <nlohmann/json.hpp>
using json = nlohmann::json;
#include "i_api_client.h"

class APIClient : public IAPIClient {
public:
  APIClient(const std::string& api_key);
  APIClient(const std::string& api_key, const std::string& cities_path);
  
  std::vector<Route> ValidateRoutes(const std::vector<Route>& routes);

  std::optional<Data> GetDataFromRequest(const Request& request) override;

  std::string GetCityCode(const std::string& city) override;
private:
  std::string api_key_;

  std::string cities_path_;
  std::unordered_map<std::string, std::string> cities_;

  void LoadCitiesFile();
  std::unordered_map<std::string, std::string> ParseCities(const json& cities);
  json GetCitiesFromApi();
  void CreateFileForCitiesAndParse();
};