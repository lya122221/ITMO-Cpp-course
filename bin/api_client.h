#pragma once
#include <string>
#include "model.h"

class APIClient {
public:
  std::optional<Data> GetDataFromRequest(const Request& request);
private:
  std::string api_key_;
};