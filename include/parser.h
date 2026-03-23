#pragma once
#include "model.h"
#include <filesystem>

class Parser {
public:
  Config ParseConfig();
private:
  std::filesystem::path GetConfigDir(const std::string& app_name);
};