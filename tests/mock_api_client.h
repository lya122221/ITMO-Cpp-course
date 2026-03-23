#pragma once
#include "i_api_client.h"
#include <gmock/gmock.h>

class MockAPIClient : public IAPIClient {
public:
  MOCK_METHOD(std::optional<Data>, GetDataFromRequest, (const Request&), (override));
  MOCK_METHOD(std::string, GetCityCode, (const std::string&), (override));
};