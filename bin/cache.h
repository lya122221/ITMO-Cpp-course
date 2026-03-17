#pragma once
#include <string>
#include <list>
#include <nlohmann/json.hpp>
using json = nlohmann::json;
#include "model.h"

struct CacheEntry {
    std::string key;
    Data data;
    int64_t timestamp;

    bool IsExpired(int64_t ttl_seconds) const {
        auto now = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
        return (now - timestamp) > ttl_seconds;
    }
};

class Cache {
public:
    std::optional<Data> Get(const std::string& from_code, const std::string& to_code, const std::string& date);
    void Put(const Data& data);
private:
    std::string file_path_;
    std::list<CacheEntry> cache_;
    size_t max_cache_size_;
    int64_t ttl_seconds_;

    bool load_file_();
    bool save_to_file_();
    std::string make_key_(const std::string& from_code, const std::string& to_code, const std::string& date) const;
};