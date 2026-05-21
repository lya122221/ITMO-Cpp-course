#pragma once
#include <unordered_map>
#include <string>
#include <variant>
#include <list>
#include <unordered_set>
#include <memory>
#include <vector>
#include <expected>
#include <chrono>
#include <utility>
#include <regex>

#include "geo_index.hpp"

namespace kvdb {

class Storage {
public:
  using Value = std::variant<
    std::string, 
    std::list<std::string>, 
    std::unordered_set<std::string>,
    GeoIndex
  >;
  using ExpiredTime = std::chrono::steady_clock::time_point;

  Storage() : max_memory_(0), curr_memory_used_(0) {}

  bool Set(const std::string& key, const Value& value, std::chrono::steady_clock::time_point ttl = std::chrono::steady_clock::time_point::max()) {
    size_t new_value_mem = CalculateMemoryUsage(key, value);
    size_t old_value_mem = 0;

    auto it = storage_.find(key);
    if (it != storage_.end()) {
      old_value_mem = CalculateMemoryUsage(key, it->second.first);
    }

    if (max_memory_ > 0 && (curr_memory_used_ - old_value_mem + new_value_mem > max_memory_)) {
      return false;
    }

    curr_memory_used_ = curr_memory_used_ - old_value_mem + new_value_mem;
    storage_[key] = { value, ttl };
    return true;
  }

  bool Del(const std::string& key) {
    auto it = storage_.find(key);
    if (it != storage_.end()) {
      curr_memory_used_ -= CalculateMemoryUsage(key, it->second.first);
      storage_.erase(it);
      return true;
    }
    return false;
  }

  bool IsExist(const std::string& key) {
    return storage_.contains(key);
  }

  void Flush() {
    storage_.clear();
    curr_memory_used_ = 0;
  }

  size_t GetKeyMemoryUsage(const std::string& key) {
    auto it = storage_.find(key);
    if (it == storage_.end()) {
      return 0;
    }

    return CalculateMemoryUsage(key, it->second.first);
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

  size_t GetMaxMem() {
    return max_memory_;
  }

  bool SetMaxMem(size_t new_mem) {
    if (new_mem > 0 && new_mem < curr_memory_used_) {
      return false;
    }

    max_memory_ = new_mem;

    return true;
  }

  size_t GetSize() {
    return storage_.size();
  }

  std::vector<std::string> GetKeysByPattern(const std::string& pattern_str) {
    std::vector<std::string> result;
    
    std::string regex_str = "^";
    
    for (char c : pattern_str) {
      if (c == '*') {
        regex_str += ".*";
      } else if (c == '?') {
        regex_str += ".";
      } else if (std::string(".+()|^$\\{}").find(c) != std::string::npos) {
        regex_str += "\\";
        regex_str += c;
      } else {
        regex_str += c;
      }
    }
    regex_str += "$";

    std::regex re;
    try {
      re = std::regex(regex_str);
    } catch (...) {
      std::cerr << "(error) Invalid pattern" << std::endl;
      return result;
    }

    for (const auto& [key, value_pair] : storage_) {
      if (!IsExpired(key)) {
        if (std::regex_match(key, re)) {
          result.push_back(key);
        }
      }
    }

    return result;
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

  size_t CalculateMemoryUsage(const std::string& key, const Value& value) {
    size_t mem = sizeof(std::pair<const std::string, std::pair<Value, ExpiredTime>>) + sizeof(void*)*2;
    mem += key.capacity();

    std::visit([&mem](auto&& arg) {
      using T = std::decay_t<decltype(arg)>;
      
      if constexpr (std::is_same_v<T, std::string>) {
        mem += arg.capacity();
      } else if constexpr (std::is_same_v<T, std::list<std::string>>) {
        for (const auto& s : arg) {
          mem += 24 + s.capacity();
        }
      } else if constexpr (std::is_same_v<T, std::unordered_set<std::string>>) {
        mem += arg.bucket_count() * sizeof(void*);
        for (const auto& s : arg) {
          mem += 24 + s.capacity();
        }
      } else if constexpr (std::is_same_v<T, GeoIndex>) {
        mem += arg.GetMemoryUsage() - sizeof(GeoIndex); 
      }
    }, value);

    return mem;
  }
};

}