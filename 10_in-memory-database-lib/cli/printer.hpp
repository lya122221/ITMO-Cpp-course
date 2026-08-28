#pragma once
#include <iostream>
#include <variant>
#include <string>
#include <list>
#include <unordered_set>
#include <vector>
#include "command.hpp"

namespace kvdb::cli {

struct ResultVisitor {
    void operator()(std::monostate) const {
        std::cout << "OK\n";
    }
    
    void operator()(const std::string& str) const {
        std::cout << "\"" << str << "\"\n";
    }
    
    void operator()(int val) const {
        std::cout << "(integer) " << val << "\n";
    }
    
    void operator()(bool val) const {
        std::cout << "(integer) " << (val ? 1 : 0) << "\n";
    }
    
    void operator()(const std::list<std::string>& list) const {
        if (list.empty()) {
            std::cout << "(empty array)\n";
            return;
        }
        int i = 1;
        for (const auto& item : list) {
            std::cout << i++ << ") \"" << item << "\"\n";
        }
    }
    
    void operator()(const std::unordered_set<std::string>& set) const {
        if (set.empty()) {
            std::cout << "(empty array)\n";
            return;
        }
        int i = 1;
        for (const auto& item : set) {
            std::cout << i++ << ") \"" << item << "\"\n";
        }
    }
    
    void operator()(const std::vector<std::string>& vec) const {
        if (vec.empty()) {
            std::cout << "(empty array)\n";
            return;
        }
        int i = 1;
        for (const auto& item : vec) {
            std::cout << i++ << ") \"" << item << "\"\n";
        }
    }
    
    void operator()(const GeoIndex& geo) const {
        std::cout << geo << "\n";
    }
};

inline void PrintResult(const CommandResult& result) {
    std::visit(ResultVisitor{}, result);
}

}