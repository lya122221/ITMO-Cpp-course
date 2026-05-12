#pragma once
#include <iostream>

#include "storage.hpp"

namespace kvdb {

using CommandResult = std::variant<
  std::monostate,
  std::string,
  int,
  std::list<std::string>,
  std::set<std::string>
>;

class Command {
public:
  virtual CommandResult Execute(std::shared_ptr<Storage> storage, std::vector<std::string> args) = 0;
  virtual ~Command() = default;
};

}