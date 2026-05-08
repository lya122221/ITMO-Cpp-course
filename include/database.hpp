#pragma once
#include <string>
#include <vector>
#include <functional>
#include <unordered_map>
#include <memory>
#include <algorithm>

#include "command.hpp"
#include "storage.hpp"

class DataBase {
public:

private:
  std::shared_ptr<Storage> storage_;
  std::unordered_map<std::string, std::unique_ptr<Command>> commands_;
};