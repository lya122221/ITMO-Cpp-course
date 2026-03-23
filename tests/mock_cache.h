#pragma once
#include "i_cache.h"
#include <gmock/gmock.h>

class MockCache : public ICache {
public:
  MOCK_METHOD(std::optional<Data>, Get, (const Request&), (override));
  MOCK_METHOD(void, Put, (const Data&), (override));
};