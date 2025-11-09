#include <cstdint>
#include <cstring>
#include <cmath>
#include <cstdlib>
#include <iostream>

namespace nargparse {

    const uint8_t kNargsRequired = 1;
    const uint8_t kNargsOptional = 2;
    const uint8_t kNargsZeroOrMore = 3;
    const uint8_t kNargsOneOrMore = 4;

    struct Flag {
        char* flag_name;
        bool* flag_value;
        char* flag_short;
        char* flag_long;
    };

    enum ArgType { ARG_INT, ARG_CHAR, ARG_FLOAT };

    struct Argument {
        char* arg_name;

        union {
            int* int_value;
            char* char_value;
            float* float_value;
        } arg_value;
        

        char* arg_short;
        char* arg_long;
        char* arg_condition;

        ArgType type;

        union {
            bool (*CheckInt)(const int&);
            bool (*CheckFloat)(const float&);
            bool (*CheckChar)(const char* const&);
        } CheckFunc;

        unsigned int repeated_count;
        size_t repeated_arr_size;
        union {
            int* int_arr;
            char** char_arr;
            float* float_arr;
        } repeated_arr;

        uint8_t kNargs;
    };
    
    struct ArgumentParser {
        const char* parser_name;

        Flag* flags;
        Argument* arguments;
        size_t flags_len;
        size_t args_len;

        int kMaxArgLen;

        int flags_count;
        int arguments_count;

        const char* parser_usage;
    };
    
    ArgumentParser CreateParser(const char* parser_name, const int kMaxArgLen = 128, const char* parser_usage = "program");
    void FreeParser(ArgumentParser& parser);
    void AddFlag(ArgumentParser& parser, char* long_flag, bool* flag, char* flag_name, bool new_flag_val = false);
    void AddFlag(ArgumentParser& parser, char* short_flag, char* long_flag, bool* flag, char* flag_name, bool new_flag_val = false);
    void AddArgument(ArgumentParser& parser,  char* short_arg, char* long_arg, int* arg_value, char* arg_name, const uint8_t kNargs = 0, bool (*CheckFunc)(const int&) = nullptr, char* condition = nullptr);
    void AddArgument(ArgumentParser& parser,  char* short_arg, char* long_arg, float* arg_value, char* arg_name, const uint8_t kNargs = 0, bool (*CheckFunc)(const float&) = nullptr, char* condition = nullptr);
    void AddArgument(ArgumentParser& parser, int* arg_value, char* arg_name, const uint8_t kNargs = 0, bool (*CheckFunc)(const int&) = nullptr, char* condition = nullptr);
    void AddArgument(ArgumentParser& parser, float* arg_value, char* arg_name, const uint8_t kNargs = 0, bool (*CheckFunc)(const float&) = nullptr, char* condition = nullptr);
    void AddArgument(ArgumentParser& parser,  char* short_arg, char* long_arg, void* arg_value, char* arg_name, const uint8_t kNargs = 0, bool (*CheckFunc)(const char* const&) = nullptr, char* condition = nullptr);
    void AddArgument(ArgumentParser& parser, void* arg_value, char* arg_name, const uint8_t kNargs = 0, bool (*CheckFunc)(const char* const&) = nullptr, char* condition = nullptr);
    bool Parse(ArgumentParser& parser, int argc, const char** argv);
    int GetRepeatedCount(ArgumentParser parser, char* arg_name);
    bool GetRepeated(ArgumentParser& parser, char* arg_name, int n, int* value);
    bool GetRepeated(ArgumentParser& parser, char* arg_name, int n, float* value);
    bool GetRepeated(ArgumentParser& parser, char* arg_name, int n, char** value);
    bool GetRepeated(ArgumentParser& parser, char* arg_name, int n, void* value);
    void AddHelp(ArgumentParser& parser);
    void PrintHelp(ArgumentParser parser);
};