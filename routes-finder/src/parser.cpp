#include "parser.h"
#include <fstream>
#include <filesystem>
#include <nlohmann/json.hpp>
using json = nlohmann::json;
#include <iostream>
#include <cstdlib>
#include <stdexcept>
#include <cstdlib>

std::filesystem::path Parser::GetConfigDir(const std::string& app_name) {
#ifdef _WIN32
    return std::filesystem::path(std::getenv("APPDATA")) / app_name;
#elif defined(__APPLE__)
    return std::filesystem::path(std::getenv("HOME")) / "Library/Application Support" / app_name;
#else
    return std::filesystem::path(std::getenv("HOME")) / ".config" / app_name;
#endif
}

Config Parser::ParseConfig() {
  std::filesystem::path config_dir = GetConfigDir("RoutesFinder");
  std::filesystem::path config_path = config_dir / "config.json";

  if (!std::filesystem::exists(config_dir)) {
    std::filesystem::create_directories(config_dir);
  }

  std::ifstream config(config_path);
  if (!config) {
    std::ofstream new_file(config_path);
    if (!new_file) {
      throw std::runtime_error("Cannot create config file");
    }
    json template_config = {
      {"api_key", "YOUR_API_KEY_HERE"},
      {"max_cache_size", 100},
      {"ttl_seconds", 3600}
    };
    new_file << template_config.dump(2);
    new_file.close();
    std::cerr << "Fill config file in this path: " << config_path << std::endl;
    exit(1);
  }

  json j = json::parse(config);

  return Config{
    j.at("api_key").get<std::string>(),
    j.at("max_cache_size").get<size_t>(),
    j.at("ttl_seconds").get<int64_t>()
  };
}