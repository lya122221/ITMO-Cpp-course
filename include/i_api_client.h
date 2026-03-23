#pragma once
#include "model.h"
#include <optional>
#include <string>

class IAPIClient {
public:
  virtual ~IAPIClient() = default;
  virtual std::optional<Data> GetDataFromRequest(const Request& request) = 0;
  virtual std::string GetCityCode(const std::string& city) = 0;
};