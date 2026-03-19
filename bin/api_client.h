#pragma once
#include <string>
#include "model.h"

class APIClient {
public:
  std::optional<Data> GetDataFromRequest(const Request& request);
private:
  std::string api_key_;

  std::vector<Route> validate_routes_(const std::vector<Route>& routes);
};