#include <cstdint>
#include <cstring>

namespace nargparse {

    const uint8_t kNargsRequired = 1;
    const uint8_t kNargsOptional = 2;
    const uint8_t kNargsZeroOrMore = 3;
    const uint8_t kNargsOneOrMore = 4;

    struct Flag {
        const char* flag_name;
        bool* flag_value;
        const char* flag_short;
        const char* flag_long;
    };

    enum ArgType { ARG_INT, ARG_CHAR, ARG_FLOAT };

    struct Argument {
        const char* arg_name;

        union {
            int* int_value;
            char* char_value;
            float* float_value;
        } arg_value;
        

        const char* arg_short;
        const char* arg_long;
        const char* arg_condition;

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
    };
    
    ArgumentParser CreateParser(const char* parser_name, const int kMaxArgLen);

    void FreeParser(ArgumentParser& parser);

    void AddFlag(ArgumentParser& parser, const char* long_flag, bool* flag,
                 const char* flag_name, bool new_flag_val = false);

    void AddFlag(ArgumentParser& parser, const char* short_flag, const char* long_flag,
                 bool* flag, const char* flag_name, bool new_flag_val = false);

    void AddArgument(ArgumentParser& parser, const char* short_arg, const char* long_arg,
                     int* arg_value, const char* arg_name, uint8_t kNargs = kNargsRequired,
                     bool (*CheckFunc)(const int&) = nullptr, const char* condition = nullptr);

    void AddArgument(ArgumentParser& parser, const char* short_arg, const char* long_arg,
                     float* arg_value, const char* arg_name, uint8_t kNargs = kNargsRequired,
                     bool (*CheckFunc)(const float&) = nullptr, const char* condition = nullptr);

    void AddArgument(ArgumentParser& parser, const char* short_arg, const char* long_arg,
                     void* arg_value, const char* arg_name, uint8_t kNargs = kNargsRequired,
                     bool (*CheckFunc)(const char* const&) = nullptr, const char* condition = nullptr);

    void AddArgument(ArgumentParser& parser, int* arg_value, const char* arg_name,
                     uint8_t kNargs = kNargsRequired,
                     bool (*CheckFunc)(const int&) = nullptr, const char* condition = nullptr);

    void AddArgument(ArgumentParser& parser, float* arg_value, const char* arg_name,
                     uint8_t kNargs = kNargsRequired,
                     bool (*CheckFunc)(const float&) = nullptr, const char* condition = nullptr);

    void AddArgument(ArgumentParser& parser, void* arg_value, const char* arg_name,
                     uint8_t kNargs = kNargsRequired,
                     bool (*CheckFunc)(const char* const&) = nullptr, const char* condition = nullptr);

    bool Parse(ArgumentParser& parser, int argc, const char** argv);

    int GetRepeatedCount(const ArgumentParser& parser, const char* arg_name);

    bool GetRepeated(ArgumentParser& parser, const char* arg_name, int n, int* value);
    bool GetRepeated(ArgumentParser& parser, const char* arg_name, int n, float* value);
    bool GetRepeated(ArgumentParser& parser, const char* arg_name, int n, char** value);
    bool GetRepeated(ArgumentParser& parser, const char* arg_name, int n, void* value);

    void AddHelp(ArgumentParser& parser);
    void PrintHelp(const ArgumentParser& parser);


};