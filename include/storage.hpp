#pragma once
#include <unordered_map>
#include <string>
#include <variant>
#include <list>
#include <set>
#include <memory>
#include <vector>
#include <optional>

class Storage {
public:
  using Value = std::variant<
    std::string, 
    std::list<std::string>, 
    std::set<std::string>
  >;
private:
  std::unordered_map<std::string, Value> storage_;

  size_t max_memory_;
  size_t curr_memory_used_;
};