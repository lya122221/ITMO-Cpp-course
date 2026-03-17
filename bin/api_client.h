#pragma once
#include <string>
#include "model.h"

class APIClient {
public:
  Data GetDataFromRequest(const std::string& from_code, const std::string& to_code, const std::string& date);
private:
  std::string api_key;
};