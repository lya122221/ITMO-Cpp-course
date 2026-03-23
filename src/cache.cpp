#include "cache.h"
#include "serialization.h"
#include <filesystem>
#include <fstream>
#include <vector>
#include <iostream>
#include <nlohmann/json.hpp>
using json = nlohmann::json;

void Cache::load_file_() {
  std::ifstream file(file_path_);
  if (!file) {
    std::ofstream new_file(file_path_);
    if (!new_file) {
      throw std::runtime_error("Cannot create cache file");
    }
    new_file << "[]";
    return; 
  }

  try {
    json file_json = json::parse(file);
    std::vector<CacheEntry> entries = file_json.get<std::vector<CacheEntry>>();

    for (CacheEntry& entry : entries) {
      if (!entry.IsExpired(ttl_seconds_)) {
        cache_.push_back(entry);
      }
    }
  } catch (const json::exception& e){
    std::cerr << "Cache load error: " << e.what() << std::endl;
    std::ofstream new_file(file_path_);
    if (!new_file) {
      throw std::runtime_error("Cannot create cache file");
    }
    new_file << "[]";
    return;
  }
}

void Cache::save_to_file_() {
  json entries = json::array();
  
  for (CacheEntry entry : cache_) {
    if (!entry.IsExpired(ttl_seconds_)) {
      entries.push_back(entry);
    }
  }

  std::ofstream file(file_path_);
  file << entries.dump();
}

std::string Cache::make_key_(const Request& request) const {
  return request.from_code + " " + request.to_code + " " + request.date;
}

std::optional<Data> Cache::Get(const Request& request) {
  std::string key = make_key_(request);

  for (auto it = cache_.begin(); it != cache_.end(); it++) {
    if (it->key == key) {
      cache_.splice(cache_.begin(), cache_, it);
      return cache_.front().data;
    }
  }

  return std::nullopt;
}

void Cache::Put(const Data& data) {
  std::string key = make_key_({ data.from_code, data.to_code, data.date });
  auto now = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();

  cache_.push_front({ key, data, now });

  while (cache_.size() > max_cache_size_) {
    cache_.pop_back();
  }
}

Cache::Cache(size_t max_cache_size, int64_t ttl_seconds) : max_cache_size_(max_cache_size), ttl_seconds_(ttl_seconds), file_path_(std::filesystem::temp_directory_path() / "routes_cache.json") {
  load_file_();
}

Cache::Cache(size_t max_cache_size, int64_t ttl_seconds, const std::string& file_path) : max_cache_size_(max_cache_size), ttl_seconds_(ttl_seconds), file_path_(file_path) {
  load_file_();
}

Cache::~Cache() {
  save_to_file_();
}