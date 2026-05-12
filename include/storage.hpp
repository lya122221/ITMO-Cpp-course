#pragma once
#include <unordered_map>
#include <string>
#include <variant>
#include <list>
#include <set>
#include <memory>
#include <vector>
#include <optional>

namespace kvdb {

class Storage {
public:
  using Value = std::variant<
    std::string, 
    std::list<std::string>, 
    std::set<std::string>
  >;

  void Set(const std::string& key, const std::string& value) {
    storage_[key] = value;
  }

  std::optional<Value> Get(const std::string& key) {
    auto key_it = storage_.find(key);
    if (key_it == storage_.end()) {
      return std::nullopt;
    }

    return key_it->second;
  }
private:
  std::unordered_map<std::string, Value> storage_;

  size_t max_memory_;
  size_t curr_memory_used_;
};

}