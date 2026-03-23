#pragma once
#include "model.h"
#include <filesystem>

class Parser {
public:
  Config ParseConfig();
private:
  std::filesystem::path get_config_dir_(const std::string& app_name);
};