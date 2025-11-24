#include <fstream>
#include <iostream>
#include "archiveoperation.h"
#include "hamcode.h"
#include <filesystem>
#include <vector>
#include <string>
#include <cstdint>
#include <cstddef>

void CreateArchive(const std::string& arc_name, const std::vector<std::string>& file_names) {
    std::ofstream archive(arc_name, std::ios::binary);
    if (!archive) {
        std::cerr << "Error: cannot create archive" << std::endl;
        return;
    }

    std::string files_str;
    for (const auto& full_path : file_names) {
        std::filesystem::path path_obj(full_path);
        std::string file_name = path_obj.filename().string();
        
        std::ifstream file_in(full_path, std::ios::binary);
        if (!file_in) {
            std::cerr << "Error: failed to open file " << full_path << std::endl;
            return;
        }

        uint16_t name_len = file_name.size();
        files_str.push_back((name_len >> 8) & 0xFF);
        files_str.push_back(name_len & 0xFF);
        files_str += file_name;

        file_in.seekg(0, std::ios::end);
        uint64_t size = file_in.tellg();
        file_in.seekg(0, std::ios::beg);
        
        for (int i = 7; i >= 0; i--) {
            files_str.push_back((size >> (8 * i)) & 0xFF);
        }

        std::string buffer(size, ' ');
        file_in.read(&buffer[0], size);
        files_str += buffer;
        
        file_in.close();
    }
    
    std::vector<unsigned char> files_vec(files_str.begin(), files_str.end());
    std::vector<unsigned char> ham_code = HamCode84(files_vec);
    archive.write((const char*)(ham_code.data()), ham_code.size());
    archive.close();
}

void ExtractAll(const std::string& arch_name) {
    std::ifstream archive(arch_name, std::ios::binary);
    if (!archive) {
        std::cerr << "Error: failed to open archive" << std::endl;
        return;
    }

    archive.seekg(0, std::ios::end);
    uint64_t size = archive.tellg();
    archive.seekg(0, std::ios::beg);

    std::vector<unsigned char> ham(size);
    archive.read((char*)(ham.data()), size);
    archive.close();

    std::vector<unsigned char> unham = UnHamCode84(ham);
    
    if (unham.empty()) {
        std::cerr << "Error: Hamming decoding failed - empty result" << std::endl;
        return;
    }

    size_t pointer = 0;
    int file_count = 0;
    
    while (pointer < unham.size()) {
        uint16_t name_len = (unham[pointer] << 8) | unham[pointer + 1];
        pointer += 2;
        
        std::string file_name((char*)(&unham[pointer]), name_len);
        pointer += name_len;

        uint64_t file_size = 0;
        for (int i = 0; i < 8; i++) {
            file_size = (file_size << 8) | unham[pointer++];
        }

        std::ofstream out_file(file_name, std::ios::binary);
        if (!out_file) {
            std::cerr << "Error: cannot create file " << file_name << std::endl;
        } else {
            out_file.write((char*)(&unham[pointer]), file_size);
            out_file.close();
            file_count++;
        }

        pointer += file_size;
    }
}

void UnhamExtract(std::vector<unsigned char>& unham, std::vector<bool>& found, const std::vector<std::string>& files_to_extract) {
    size_t pointer = 0;

    while (pointer < unham.size()) {
        uint16_t name_len = (unham[pointer] << 8) | unham[pointer + 1];
        pointer += 2;

        std::string curr_file((char*)(&unham[pointer]), name_len);
        pointer += name_len;

        uint64_t file_size = 0;
        for (int i = 0; i < 8; i++) {
            file_size = (file_size << 8) | unham[pointer++];
        }

        bool should_extract = false;
        size_t index = 0;
        for (size_t i = 0; i < files_to_extract.size(); ++i) {
            if (files_to_extract[i] == curr_file) {
                should_extract = true;
                index = i;
                break;
            }
        }

        if (should_extract) {
            std::ofstream out_file(curr_file, std::ios::binary);
            if (!out_file) {
                std::cerr << "Error: cannot create file " << curr_file << std::endl;
            } else {
                out_file.write((char*)(&unham[pointer]), file_size);
                out_file.close();
                found[index] = true;
            }
        }

        pointer += file_size;
    }
}

void ExtractFiles(const std::string& arch_name, const std::vector<std::string>& files_to_extract) {
    std::ifstream archive(arch_name, std::ios::binary);

    if (!archive) {
        std::cerr << "Error: failed to open archive" << std::endl;
        return;
    }

    archive.seekg(0, std::ios::end);
    size_t size = archive.tellg();
    archive.seekg(0, std::ios::beg);

    std::vector<unsigned char> ham(size);
    archive.read((char*)(ham.data()), size);
    archive.close();

    std::vector<unsigned char> unham = UnHamCode84(ham);
    if (unham.empty()) {
        std::cerr << "Error: Hamming decoding failed" << std::endl;
        return;
    }

    std::vector<bool> found(files_to_extract.size(), false);

    UnhamExtract(unham, found, files_to_extract);

    for (size_t i = 0; i < files_to_extract.size(); i++) {
        if (!found[i]) {
            std::cerr << "Warning: file " << files_to_extract[i] << " not found in archive" << std::endl;
        }
    }
}

void MergeArchives(const std::string& first_arch, const std::string& second_arch, const std::string& new_name) {
    std::ifstream archive1(first_arch, std::ios::binary);
    std::ifstream archive2(second_arch, std::ios::binary);

    if (!archive1 || !archive2) {
        std::cerr << "Error: failed to open one of the archives" << std::endl;
        return;
    }

    archive1.seekg(0, std::ios::end);
    size_t size1 = archive1.tellg();
    archive1.seekg(0, std::ios::beg);

    archive2.seekg(0, std::ios::end);
    size_t size2 = archive2.tellg();
    archive2.seekg(0, std::ios::beg);

    std::vector<unsigned char> ham_arch1(size1);
    std::vector<unsigned char> ham_arch2(size2);

    archive1.read((char*)(ham_arch1.data()), size1);
    archive2.read((char*)(ham_arch2.data()), size2);

    archive1.close();
    archive2.close();

    std::vector<unsigned char> arch1 = UnHamCode84(ham_arch1);
    std::vector<unsigned char> arch2 = UnHamCode84(ham_arch2);

    std::string combined_data;
    combined_data.reserve(arch1.size() + arch2.size());
    combined_data.assign(arch1.begin(), arch1.end());
    combined_data.append(arch2.begin(), arch2.end());

    std::vector<unsigned char> combined_vec(combined_data.begin(), combined_data.end());
    std::vector<unsigned char> ham_code = HamCode84(combined_vec);

    std::ofstream archive_new(new_name, std::ios::binary | std::ios::trunc);

    if (!archive_new) {
        std::cerr << "Error: cannot create new archive" << std::endl;
        return;
    }

    archive_new.write((const char*)(ham_code.data()), ham_code.size());
    archive_new.close();
}