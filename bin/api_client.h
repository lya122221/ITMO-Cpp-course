#pragma once
#include <string>
#include "model.h"

class APIClient {
public:
  APIClient(const std::string& api_key) : api_key_(api_key) {}

  std::optional<Data> GetDataFromRequest(const Request& request);
private:
  std::string api_key_;

  std::vector<Route> validate_routes_(const std::vector<Route>& routes);
  std::string find_api_key();
};