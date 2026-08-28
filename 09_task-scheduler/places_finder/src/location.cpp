#include <cpr/cpr.h>
#include <nlohmann/json.hpp>
using json = nlohmann::json;
#include "model.h"
#include <string>
#include <iostream>

Location GetLocation() {
  cpr::Response response = cpr::Get(
    cpr::Url{"https://api.2ip.io/"},
    cpr::Proxies{{"http", ""}, {"https", ""}},
    cpr::Parameters{
      {"token", IP_API_KEY}
    }
  );

  if (response.status_code != 200) {
    throw std::runtime_error("Error 2ip API");
  }

  json data = json::parse(response.text);
  return { std::stod(data["lat"].get<std::string>()), std::stod(data["lon"].get<std::string>()) };
}