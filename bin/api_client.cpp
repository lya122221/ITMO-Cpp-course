#include "api_client.h"
#include "model.h"
#include <cpr/cpr.h>
#include <nlohmann/json.hpp>
using json = nlohmann::json;
#include "serialization.h"
#include <iostream>

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
  data.routes = validate_routes_(data.routes);
  return data;
}

std::vector<Route> APIClient::validate_routes_(const std::vector<Route>& routes) {
  std::vector<Route> validated_routes;
  for (auto it = routes.begin(); it != routes.end(); it++) {
    if ((it->segments).size() < 3) {
      validated_routes.push_back(*it);
    }
  }

  return validated_routes;
}