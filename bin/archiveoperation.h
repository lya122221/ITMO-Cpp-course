#include <vector>
#include <string>

void CreateArchive(const std::string& arc_name, const std::vector<std::string>& file_names);
void ExtractAll(const std::string& arch_name);
void ExtractFiles(const std::string& arch_name, const std::vector<std::string>& files_to_extract);
void MergeArchives(const std::string& first_arch, const std::string& second_arch, const std::string& new_name);
