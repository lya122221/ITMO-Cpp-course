#pragma once
#include <string>
#include <cctype>
#include <stdexcept>
#include <vector>
#include <sstream>

namespace kvdb::cli {

inline size_t ParseMemoryLimit(const std::string& mem_str) {
    size_t multiplier = 1;
    std::string number_part;
    std::string suffix;

    for (char c : mem_str) {
        if (std::isdigit(c)) {
            number_part += c;
        } else {
            suffix += std::tolower(c);
        }
    }

    if (suffix == "b" || suffix == "") {
        multiplier = 1;
    } else if (suffix == "kb") {
        multiplier = 1024;
    } else if (suffix == "mb") {
        multiplier = 1024 * 1024;
    } else if (suffix == "gb") {
        multiplier = 1024 * 1024 * 1024;
    }
    else { throw std::invalid_argument("Invalid memory suffix"); }

    return std::stoull(number_part) * multiplier;
}

inline std::vector<std::string> ParseCommandString(const std::string& line) {
    std::vector<std::string> args;
    std::istringstream iss(line);
    std::string word;
    
    while (iss >> word) {
        args.push_back(word);
    }
    return args;
}

}