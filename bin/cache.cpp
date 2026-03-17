#include "cache.h"
#include "serialization.h"
#include <fstream>
#include <vector>
#include <iostream>

bool Cache::load_file_() {
  std::ifstream file(file_path_);
  if (!file) {
    return false;
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
  }
}

bool Cache::save_to_file_() {
  json entries = json::array();
  
  for (CacheEntry entry : cache_) {
    if (!entry.IsExpired(ttl_seconds_)) {
      entries.push_back(entry);
    }
  }

  std::ofstream file(file_path_);
  file << entries.dump();
}

std::string Cache::make_key_(const std::string& from_code, const std::string& to_code, const std::string& date) const {
  return from_code + " " + to_code + " " + date;
}

std::optional<Data> Cache::Get(const std::string& from_code, const std::string& to_code, const std::string& date) {
  std::string key = make_key_(from_code, to_code, date);

  for (auto it = cache_.begin(); it != cache_.end(); it++) {
    if (it->key == key) {
      cache_.splice(cache_.begin(), cache_, it);
      return cache_.front().data;
    }
  }

  return std::nullopt;
}

void Cache::Put(const Data& data) {
  std::string key = make_key_(data.from_code, data.to_code, data.date);
  auto now = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();

  cache_.push_front({ key, data, now });

  while (cache_.size() > max_cache_size_) {
    cache_.pop_back();
  }
}