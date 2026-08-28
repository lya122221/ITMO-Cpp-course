#pragma once
#include <iostream>
#include <unordered_set>

#include "storage.hpp"
#include "geo_index.hpp"

namespace kvdb {

using CommandResult = std::variant<
  std::monostate,
  std::string,
  int,
  std::list<std::string>,
  std::unordered_set<std::string>,
  bool,
  std::vector<std::string>,
  GeoIndex
>;

class Command {
public:
  virtual CommandResult Execute(std::shared_ptr<Storage> storage, std::vector<std::string> args) = 0;
  virtual ~Command() = default;
};

}