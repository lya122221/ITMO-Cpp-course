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
    entries.push_back(entry);
  }

  std::ofstream file(file_path_);
  file << entries.dump();
}