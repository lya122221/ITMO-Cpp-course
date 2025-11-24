#pragma once
#include <string>

void ListFiles(const std::string& arch_name);
void AppendFile(const std::string& arch_name, const std::string& file_name);
void DeleteFile(const std::string& arch_name, const std::string& target_file_name);
