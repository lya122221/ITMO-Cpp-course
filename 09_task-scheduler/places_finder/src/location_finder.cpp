#include <cpr/cpr.h>
#include <nlohmann/json.hpp>
using json = nlohmann::json;
#include "include/model.h"

Location GetLocation() {
  cpr::Response response = cpr::Get(
    cpr::Url{"https://api.2ip.io/"},
    cpr::Parameters{
      {"token", location_api_key}
    }
  );

  if (response.status_code != 200) {
    throw std::runtime_error("Error 2ip API");
  }

  json data = json::parse(response.text);
  return { data["lat"].get<double>(), data["lon"].get<double>() };
}