#pragma once
#include "model.h"
#include <optional>

class ICache {
public:
  virtual ~ICache() = default;
  virtual std::optional<Data> Get(const Request& request) = 0;
  virtual void Put(const Data& data) = 0;
};