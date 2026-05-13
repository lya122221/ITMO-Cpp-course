#pragma once
#include <unordered_map>
#include <string>
#include <variant>
#include <list>
#include <set>
#include <memory>
#include <vector>
#include <expected>
#include <chrono>
#include <utility>

namespace kvdb {

class Storage {
public:
  using Value = std::variant<
    std::string, 
    std::list<std::string>, 
    std::set<std::string>
  >;
  using ExpiredTime = std::chrono::steady_clock::time_point;

  void Set(const std::string& key, const Value& value, std::chrono::steady_clock::time_point ttl = std::chrono::steady_clock::time_point::max()) {
    storage_[key] = { value, ttl };
  }

  std::expected<Value, std::string> Get(const std::string& key) {
    auto key_it = storage_.find(key);
    if (key_it == storage_.end()) {
      return std::unexpected("(error) Invalid key");
    }

    if (IsExpired(key)) {
      return std::unexpected("(error) Key expired");
    }

    return (key_it->second).first;
  }

  std::chrono::seconds GetTtl(const std::string& key) {
    auto key_it = storage_.find(key);
    if (key_it == storage_.end()) {
      return std::chrono::seconds(0);
    }

    ExpiredTime expired_time = (key_it->second).second;
    if (expired_time != std::chrono::steady_clock::time_point::max()) {
      return std::chrono::duration_cast<std::chrono::seconds>(expired_time - std::chrono::steady_clock::now());
    } else {
      return std::chrono::seconds(0);
    }
  }

private:
  std::unordered_map<std::string, std::pair<Value, ExpiredTime>> storage_;

  size_t max_memory_;
  size_t curr_memory_used_;

  bool IsExpired(const std::string& key) {
    auto now = std::chrono::steady_clock::now();

    if (now >= storage_[key].second) {
      return true;
    } else {
      return false;
    }
  }
};

}