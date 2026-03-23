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

  std::optional<Data> GetDataFromRequest(const Request& request) override;

  std::string GetCityCode(const std::string& city) override;
private:
  std::string api_key_;

  std::string cities_path_;
  std::unordered_map<std::string, std::string> cities_;

  std::vector<Route> validate_routes_(const std::vector<Route>& routes);

  void load_cities_file_();
  std::unordered_map<std::string, std::string> parse_cities_(const json& cities);
  json get_cities_from_api_();
  void create_file_for_cities_and_parse_();
};