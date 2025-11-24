#include <vector>
#include <string>
#include <cstdint>
#include <fstream>
#include <iostream>
#include "filesoperation.h"
#include "hamcode.h"
#include <filesystem>


void ListFiles(const std::string& arch_name) {
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
        std::cerr << "Error: Hamming decoding failed" << std::endl;
        return;
    }

    size_t pointer = 0;
    while (pointer < unham.size()) {
        uint16_t name_len = (unham[pointer] << 8) | unham[pointer + 1];
        pointer += 2;

        std::string file_name((char*)(&unham[pointer]), name_len);
        pointer += name_len;

        uint64_t file_size = 0;
        for (int i = 0; i < 8; i++) {
            file_size = (file_size << 8) | unham[pointer++];
        }

        std::cout << file_name << std::endl;
        pointer += file_size;
    }
}

void AppendFile(const std::string& arch_name, const std::string& file_name) {
    std::ifstream archive(arch_name, std::ios::binary);
    std::string decompressed_arch;

    if (archive) {
        archive.seekg(0, std::ios::end);
        uint64_t size = archive.tellg();
        archive.seekg(0, std::ios::beg);
        std::vector<unsigned char> ham(size);
        archive.read((char*)(ham.data()), size);
        archive.close();

        std::vector<unsigned char> unham = UnHamCode84(ham);
        if (unham.empty()) {
            std::cerr << "Error: Hamming decoding failed" << std::endl;
            return;
        }
        decompressed_arch.assign(unham.begin(), unham.end());
    }

    std::ifstream file_in(file_name, std::ios::binary);
    if (!file_in) {
        std::cerr << "Error: failed to open file " << file_name << std::endl;
        return;
    }

    std::filesystem::path path_obj(file_name);
    std::string short_name = path_obj.filename().string();
    
    uint16_t name_len = short_name.size();
    decompressed_arch.push_back((name_len >> 8) & 0xFF);
    decompressed_arch.push_back(name_len & 0xFF);
    decompressed_arch += short_name;

    file_in.seekg(0, std::ios::end);
    uint64_t size = file_in.tellg();
    file_in.seekg(0, std::ios::beg);

    for (int i = 7; i >= 0; i--) {
        decompressed_arch.push_back((size >> (8 * i)) & 0xFF);
    }

    std::string buffer(size, ' ');
    file_in.read(&buffer[0], size);
    decompressed_arch += buffer;

    file_in.close();

    std::vector<unsigned char> files_vec(decompressed_arch.begin(), decompressed_arch.end());
    std::vector<unsigned char> ham_code = HamCode84(files_vec);
    std::ofstream archive_out(arch_name, std::ios::binary | std::ios::trunc);
    archive_out.write((const char*)(ham_code.data()), ham_code.size());
    archive_out.close();
}

std::string FindAndDel(const std::string& archive_str, const std::string& target_file_name, bool& found) {
    size_t pointer = 0;
    std::string new_content;

    while (pointer < archive_str.size()) {
        uint16_t name_len = (archive_str[pointer] << 8) | (archive_str[pointer + 1]);
        pointer += 2;

        std::string curr_name = archive_str.substr(pointer, name_len);
        pointer += name_len;

        uint64_t file_size = 0;
        for (int i = 0; i < 8; i++) {
            file_size = (file_size << 8) | (unsigned char)(archive_str[pointer++]);
        }
        
        std::string file_data = archive_str.substr(pointer, file_size);
        pointer += file_size;

        if (curr_name != target_file_name) {
            new_content.push_back((name_len >> 8) & 0xFF);
            new_content.push_back(name_len & 0xFF);
            new_content += curr_name;
            for (int i = 7; i >= 0; i--) {
                new_content.push_back((file_size >> (8 * i)) & 0xFF);
            }
            new_content += file_data;
        } else {
            found = true;
        }
    }

    return new_content;
}

void DeleteFile(const std::string& arch_name, const std::string& target_file_name) {
    std::ifstream archive(arch_name, std::ios::binary);
    if (!archive) {
        std::cerr << "Error: cannot open archive " << arch_name << std::endl;
        return;
    }
    archive.seekg(0, std::ios::end);
    uint64_t size = archive.tellg();
    archive.seekg(0, std::ios::beg);

    std::vector<unsigned char> ham(size);
    archive.read((char*)(ham.data()), size);
    archive.close();

    std::vector<unsigned char> decoded = UnHamCode84(ham);
    if (decoded.empty()) {
        std::cerr << "Error: Hamming decoding failed" << std::endl;
        return;
    }

    std::string archive_str(decoded.begin(), decoded.end());

    bool found = false;
    std::string new_content = FindAndDel(archive_str, target_file_name, found);

    if (!found) {
        std::cerr << "Warning: file '" << target_file_name << "' not found in archive" << std::endl;
        return;
    }

    std::vector<unsigned char> files_vec(new_content.begin(), new_content.end());
    std::vector<unsigned char> ham_code = HamCode84(files_vec);
    std::ofstream archive_out(arch_name, std::ios::binary | std::ios::trunc);
    archive_out.write((const char*)(ham_code.data()), ham_code.size());
    archive_out.close();
}