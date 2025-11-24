#include <iostream>
#include "hamcode.h"

std::vector<unsigned char> HamCode84(const std::vector<unsigned char>& data) {
    std::vector<unsigned char> result;
    
    for (unsigned char byte : data) {
        for (int half = 0; half < 2; half++) {
            int nibble = (byte >> (4 * (1 - half))) & 0xF;
            
            uint8_t d1 = (nibble >> 3) & 1;
            uint8_t d2 = (nibble >> 2) & 1;
            uint8_t d3 = (nibble >> 1) & 1;
            uint8_t d4 = nibble & 1;
            
            uint8_t p1 = d1 ^ d2 ^ d4;
            uint8_t p2 = d1 ^ d3 ^ d4;
            uint8_t p3 = d2 ^ d3 ^ d4;
            uint8_t p4 = d1 ^ d2 ^ d3 ^ d4 ^ p1 ^ p2 ^ p3;
            
            unsigned char encoded = (d1 << 7) | (d2 << 6) | (d3 << 5) | (d4 << 4) | (p1 << 3) | (p2 << 2) | (p3 << 1) | p4;
            
            result.push_back(encoded);
        }
    }
    return result;
}

std::vector<unsigned char> UnHamCode84(const std::vector<unsigned char>& ham_code) {
    std::vector<unsigned char> result;
    
    for (size_t i = 0; i < ham_code.size(); i++) {
        unsigned char byte = ham_code[i];
        
        uint8_t d1 = (byte >> 7) & 1;
        uint8_t d2 = (byte >> 6) & 1;
        uint8_t d3 = (byte >> 5) & 1;
        uint8_t d4 = (byte >> 4) & 1;
        uint8_t p1 = (byte >> 3) & 1;
        uint8_t p2 = (byte >> 2) & 1;
        uint8_t p3 = (byte >> 1) & 1;
        uint8_t p4 = byte & 1;
        

        uint8_t s1 = p1 ^ d1 ^ d2 ^ d4;
        uint8_t s2 = p2 ^ d1 ^ d3 ^ d4;
        uint8_t s3 = p3 ^ d2 ^ d3 ^ d4;
        uint8_t s4 = p4 ^ d1 ^ d2 ^ d3 ^ d4 ^ p1 ^ p2 ^ p3;
        
        uint8_t error_bit = 0;
        if (s1 == 1 && s2 == 1 && s3 == 0) error_bit = 7;
        else if (s1 == 1 && s2 == 0 && s3 == 1) error_bit = 6;
        else if (s1 == 0 && s2 == 1 && s3 == 1) error_bit = 5;
        else if (s1 == 1 && s2 == 1 && s3 == 1) error_bit = 4;
        else if (s1 == 1 && s2 == 0 && s3 == 0) error_bit = 3;
        else if (s1 == 0 && s2 == 1 && s3 == 0) error_bit = 2;
        else if (s1 == 0 && s2 == 0 && s3 == 1) error_bit = 1;
        else if (s4 == 1 && s1 == 0 && s2 == 0 && s3 == 0) error_bit = 0;
        
        if (error_bit != 0 || (s4 != 0 && s1 == 0 && s2 == 0 && s3 == 0)) {
            if (error_bit == 0) {
                byte ^= (1 << 0);
            } else {
                byte ^= (1 << error_bit);
            }
        } else if (s4 != 0) {
            if (s1 != 0 || s2 != 0 || s3 != 0 || s4 != 0) {
                std::cout << " Detected double error, cannot correct";
            }
        }
        
        d1 = (byte >> 7) & 1;
        d2 = (byte >> 6) & 1;
        d3 = (byte >> 5) & 1;
        d4 = (byte >> 4) & 1;
        
        unsigned char nibble = (d1 << 3) | (d2 << 2) | (d3 << 1) | d4;
        
        if (i % 2 == 0) {
            result.push_back(nibble << 4);
        } else {
            result.back() |= nibble;
        }
    }
    
    return result;
}