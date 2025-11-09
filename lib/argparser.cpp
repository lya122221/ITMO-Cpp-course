#include "argparser.h"

namespace nargparse {

ArgumentParser CreateParser(const char* parser_name, const int kMaxArgLen) {
    ArgumentParser res = {};
    res.parser_name = parser_name;
    res.kMaxArgLen = kMaxArgLen;
    res.arguments_count = 0;
    res.flags_count = 0;
    res.flags_len = 128;
    res.args_len = 128;
    res.arguments = new Argument[128]();
    res.flags = new Flag[128]();
    return res;
}

void FreeParser(ArgumentParser& parser) {
    delete[] parser.flags;
    for (int i = 0; i < parser.arguments_count; i++) {
        if (parser.arguments[i].type == ARG_INT) {
            delete[] parser.arguments[i].repeated_arr.int_arr;
        } else if (parser.arguments[i].type == ARG_FLOAT) {
            delete[] parser.arguments[i].repeated_arr.float_arr;
        } else if (parser.arguments[i].type == ARG_CHAR) {
            if (parser.arguments[i].repeated_count > 1) {
                for (int k = 0; k < parser.arguments[i].repeated_count - 1; ++k) {
                    delete[] parser.arguments[i].repeated_arr.char_arr[k];
                }
            }
            delete[] parser.arguments[i].repeated_arr.char_arr;
        }
    }
    delete[] parser.arguments;
    parser.parser_name = nullptr;
    parser.kMaxArgLen = 0;
    parser.arguments_count = 0;
    parser.flags_count = 0;
    parser.flags_len = 0;
    parser.args_len = 0;
}

void AddFlag(ArgumentParser& parser, const char* long_flag, bool* flag, const char* flag_name, bool new_flag_val) {
    *flag = new_flag_val;
    parser.flags_count++;
    int flags_count = parser.flags_count;

    if (flags_count <= parser.flags_len) {
        parser.flags[flags_count - 1] = {flag_name, flag, nullptr, long_flag};
    } else {
        size_t new_flags_len = parser.flags_len * 2;
        Flag* new_flags = new Flag[new_flags_len]();
        for (int i = 0; i < flags_count - 1; i++) {
            new_flags[i] = parser.flags[i];
        }
        delete[] parser.flags;
        new_flags[flags_count - 1] = {flag_name, flag, nullptr, long_flag};
        parser.flags = new_flags;
        parser.flags_len = new_flags_len;
    }
}

void AddFlag(ArgumentParser& parser, const char* short_flag, const char* long_flag, bool* flag, const char* flag_name, bool new_flag_val) {
    *flag = new_flag_val;
    parser.flags_count++;
    int flags_count = parser.flags_count;

    if (flags_count <= parser.flags_len) {
        parser.flags[flags_count - 1] = {flag_name, flag, short_flag, long_flag};
    } else {
        size_t new_flags_len = parser.flags_len * 2;
        Flag* new_flags = new Flag[new_flags_len]();
        for (int i = 0; i < flags_count - 1; i++) {
            new_flags[i] = parser.flags[i];
        }
        delete[] parser.flags;
        new_flags[flags_count - 1] = {flag_name, flag, short_flag, long_flag};
        parser.flags = new_flags;
        parser.flags_len = new_flags_len;
    }
}

void CreateNewArgs(ArgumentParser& parser, Argument new_arg) {
    int arg_count = parser.arguments_count;
    if (arg_count < parser.args_len) {
        parser.arguments[arg_count] = new_arg;
    } else {
        size_t new_args_len = parser.args_len * 2;
        Argument* new_args = new Argument[new_args_len]();
        for(int i = 0; i < arg_count; i++) {
            new_args[i] = parser.arguments[i];
        }
        delete[] parser.arguments;
        new_args[arg_count] = new_arg;
        parser.arguments = new_args;
        parser.args_len = new_args_len;
    }
    parser.arguments_count++;
}

void AddArgument(ArgumentParser& parser, const char* short_arg, const char* long_arg, int* arg_value, const char* arg_name, const uint8_t kNargs, bool (*CheckFunc)(const int&), const char* condition) {
    Argument new_arg = {};
    new_arg.arg_name = arg_name;
    new_arg.arg_value.int_value = arg_value;
    new_arg.arg_short = short_arg;
    new_arg.arg_long = long_arg;
    new_arg.arg_condition = condition;
    new_arg.type = ARG_INT;
    new_arg.CheckFunc.CheckInt = CheckFunc;
    new_arg.kNargs = kNargs;
    new_arg.repeated_arr_size = 1;
    new_arg.repeated_arr.int_arr = new int[1]();
    new_arg.repeated_count = 0;
    nargparse::CreateNewArgs(parser, new_arg);
}

void AddArgument(ArgumentParser& parser, const char* short_arg, const char* long_arg, float* arg_value, const char* arg_name, const uint8_t kNargs, bool (*CheckFunc)(const float&), const char* condition) {
    Argument new_arg = {};
    new_arg.arg_name = arg_name;
    new_arg.arg_value.float_value = arg_value;
    new_arg.arg_short = short_arg;
    new_arg.arg_long = long_arg;
    new_arg.arg_condition = condition;
    new_arg.type = ARG_FLOAT;
    new_arg.CheckFunc.CheckFloat = CheckFunc;
    new_arg.kNargs = kNargs;
    new_arg.repeated_arr_size = 1;
    new_arg.repeated_arr.float_arr = new float[1]();
    new_arg.repeated_count = 0;
    nargparse::CreateNewArgs(parser, new_arg);
}

void AddArgument(ArgumentParser& parser, int* arg_value, const char* arg_name, const uint8_t kNargs, bool (*CheckFunc)(const int&), const char* condition) {
    Argument new_arg = {};
    new_arg.arg_name = arg_name;
    new_arg.arg_value.int_value = arg_value;
    new_arg.arg_short = nullptr;
    new_arg.arg_long = nullptr;
    new_arg.arg_condition = condition;
    new_arg.type = ARG_INT;
    new_arg.CheckFunc.CheckInt = CheckFunc;
    new_arg.kNargs = kNargs;
    new_arg.repeated_arr_size = 1;
    new_arg.repeated_arr.int_arr = new int[1]();
    new_arg.repeated_count = 0;
    nargparse::CreateNewArgs(parser, new_arg);
}

void AddArgument(ArgumentParser& parser, float* arg_value, const char* arg_name, const uint8_t kNargs, bool (*CheckFunc)(const float&), const char* condition) {
    Argument new_arg = {};
    new_arg.arg_name = arg_name;
    new_arg.arg_value.float_value = arg_value;
    new_arg.arg_short = nullptr;
    new_arg.arg_long = nullptr;
    new_arg.arg_condition = condition;
    new_arg.type = ARG_FLOAT;
    new_arg.CheckFunc.CheckFloat = CheckFunc;
    new_arg.kNargs = kNargs;
    new_arg.repeated_arr_size = 1;
    new_arg.repeated_arr.float_arr = new float[1]();
    new_arg.repeated_count = 0;
    nargparse::CreateNewArgs(parser, new_arg);
}

void AddArgument(ArgumentParser& parser, const char* short_arg, const char* long_arg, void* arg_value, const char* arg_name, const uint8_t kNargs, bool (*CheckFunc)(const char* const&), const char* condition) {
    char* buffer = static_cast<char*>(arg_value);
    Argument new_arg = {};
    new_arg.arg_name = arg_name;
    new_arg.arg_value.char_value = buffer;
    new_arg.arg_short = short_arg;
    new_arg.arg_long = long_arg;
    new_arg.arg_condition = condition;
    new_arg.type = ARG_CHAR;
    new_arg.CheckFunc.CheckChar = CheckFunc;
    new_arg.kNargs = kNargs;
    new_arg.repeated_arr_size = 1;
    new_arg.repeated_arr.char_arr = new char*[1]();
    new_arg.repeated_count = 0;
    nargparse::CreateNewArgs(parser, new_arg);
}

void AddArgument(ArgumentParser& parser, void* arg_value, const char* arg_name, const uint8_t kNargs, bool (*CheckFunc)(const char* const&), const char* condition) {
    char* buffer = static_cast<char*>(arg_value);
    Argument new_arg = {};
    new_arg.arg_name = arg_name;
    new_arg.arg_value.char_value = buffer;
    new_arg.arg_short = nullptr;
    new_arg.arg_long = nullptr;
    new_arg.arg_condition = condition;
    new_arg.type = ARG_CHAR;
    new_arg.CheckFunc.CheckChar = CheckFunc;
    new_arg.kNargs = kNargs;
    new_arg.repeated_arr_size = 1;
    new_arg.repeated_arr.char_arr = new char*[1]();
    new_arg.repeated_count = 0;
    nargparse::CreateNewArgs(parser, new_arg);
}

bool CheckInt(const char* str) {
    if (!str || *str == '\0') return false;
    char* end;
    std::strtol(str, &end, 10);
    return *end == '\0';
}

bool CheckFloat(const char* str) {
    if (!str || *str == '\0') return false;
    char* end;
    std::strtof(str, &end);
    return *end == '\0';
}

bool ArgToParser(ArgumentParser& parser, Argument* arg, int curr) {
    if (arg->CheckFunc.CheckInt != nullptr && !arg->CheckFunc.CheckInt(curr)) {
        return false;
    }
    if (arg->repeated_count > 0) {
        if (arg->kNargs != nargparse::kNargsZeroOrMore && arg->kNargs != nargparse::kNargsOneOrMore) {
            return false;
        }
    }
    if (arg->repeated_count == 0) {
        *(arg->arg_value.int_value) = curr;
        arg->repeated_count = 1;
    } else {
        if (arg->repeated_count - 1 >= arg->repeated_arr_size) {
            size_t new_size = arg->repeated_arr_size * 2;
            int* new_arr = new int[new_size]();
            for (int i = 0; i < arg->repeated_count - 1; i++) {
                new_arr[i] = arg->repeated_arr.int_arr[i];
            }
            delete[] arg->repeated_arr.int_arr;
            arg->repeated_arr.int_arr = new_arr;
            arg->repeated_arr_size = new_size;
        }
        arg->repeated_arr.int_arr[arg->repeated_count - 1] = curr;
        arg->repeated_count++;
    }
    return true;
}

bool ArgToParser(ArgumentParser& parser, Argument* arg, float curr) {
    if (arg->CheckFunc.CheckFloat != nullptr && !arg->CheckFunc.CheckFloat(curr)) {
        return false;
    }
    if (arg->repeated_count > 0) {
        if (arg->kNargs != nargparse::kNargsZeroOrMore && arg->kNargs != nargparse::kNargsOneOrMore) {
            return false;
        }
    }
    if (arg->repeated_count == 0) {
        *(arg->arg_value.float_value) = curr;
        arg->repeated_count = 1;
    } else {
        if (arg->repeated_count - 1 >= arg->repeated_arr_size) {
            size_t new_size = arg->repeated_arr_size * 2;
            float* new_arr = new float[new_size]();
            for (int i = 0; i < arg->repeated_count - 1; ++i) {
                new_arr[i] = arg->repeated_arr.float_arr[i];
            }
            delete[] arg->repeated_arr.float_arr;
            arg->repeated_arr.float_arr = new_arr;
            arg->repeated_arr_size = new_size;
        }
        arg->repeated_arr.float_arr[arg->repeated_count - 1] = curr;
        arg->repeated_count++;
    }
    return true;
}

bool ArgToParser(ArgumentParser& parser, Argument* arg, const char* curr) {
    if (arg->CheckFunc.CheckChar && !arg->CheckFunc.CheckChar(curr)) {
        return false;
    }
    size_t len = std::strlen(curr);
    if (static_cast<int>(len) >= parser.kMaxArgLen) {
        return false;
    }
    if (arg->repeated_count > 0) {
        if (arg->kNargs != nargparse::kNargsZeroOrMore && arg->kNargs != nargparse::kNargsOneOrMore) {
            return false;
        }
    }
    if (arg->repeated_count == 0) {
        std::strncpy(arg->arg_value.char_value, curr, parser.kMaxArgLen - 1);
        arg->arg_value.char_value[parser.kMaxArgLen - 1] = '\0';
        arg->repeated_count = 1;
    } else {
        if (arg->repeated_count - 1 < arg->repeated_arr_size) {
            char* copy = new char[len + 1];
            std::strcpy(copy, curr);
            arg->repeated_arr.char_arr[arg->repeated_count - 1] = copy;
        } else {
            size_t new_size = arg->repeated_arr_size * 2;
            char** new_arr = new char*[new_size]();
            for (int i = 0; i < arg->repeated_count - 1; ++i) {
                new_arr[i] = arg->repeated_arr.char_arr[i];
            }
            delete[] arg->repeated_arr.char_arr;
            char* copy = new char[len + 1];
            std::strcpy(copy, curr);
            new_arr[arg->repeated_count - 1] = copy;
            arg->repeated_arr.char_arr = new_arr;
            arg->repeated_arr_size = new_size;
        }
        arg->repeated_count++;
    }
    return true;
}

bool ArgProccess(ArgumentParser& parser, int argc, const char** argv, const char** arg_name, const char** flag_name, const char* curr_arg, int& i) {
    int arg_index = -1;
    int flag_index = -1;

    const char* equal_pos = std::strchr(curr_arg, '=');
    const char* arg_key = curr_arg;
    const char* arg_val = nullptr;
    char* key_buf = nullptr;

    if (equal_pos != nullptr) {
        size_t key_len = equal_pos - curr_arg;
        key_buf = new char[key_len + 1];
        std::strncpy(key_buf, curr_arg, key_len);
        key_buf[key_len] = '\0';
        arg_key = key_buf;
        arg_val = equal_pos + 1;
    }

    if (arg_key[0] == '-' && arg_key[1] == '-') {
        for (int j = 0; j < parser.arguments_count; ++j) {
            if (parser.arguments[j].arg_long != nullptr && strcmp(arg_key, parser.arguments[j].arg_long) == 0) {
                if (*arg_name != nullptr || *flag_name != nullptr) {
                    delete[] key_buf;
                    return false;
                }
                *arg_name = parser.arguments[j].arg_name;
                arg_index = j;
                break;
            }
        }
        if (arg_index == -1) {
            for (int j = 0; j < parser.flags_count; ++j) {
                if (parser.flags[j].flag_long != nullptr && strcmp(arg_key, parser.flags[j].flag_long) == 0) {
                    if (*flag_name != nullptr || *arg_name != nullptr) {
                        delete[] key_buf;
                        return false;
                    }
                    *flag_name = parser.flags[j].flag_name;
                    flag_index = j;
                    break;
                }
            }
        }
    } else if (arg_key[0] == '-') {
        for (int j = 0; j < parser.arguments_count; ++j) {
            if (parser.arguments[j].arg_short != nullptr && strcmp(arg_key, parser.arguments[j].arg_short) == 0) {
                if (*arg_name != nullptr || *flag_name != nullptr) {
                    delete[] key_buf;
                    return false;
                }
                *arg_name = parser.arguments[j].arg_name;
                arg_index = j;
                break;
            }
        }
        if (arg_index == -1) {
            for (int j = 0; j < parser.flags_count; ++j) {
                if (parser.flags[j].flag_short != nullptr && strcmp(arg_key, parser.flags[j].flag_short) == 0) {
                    if (*flag_name != nullptr || *arg_name != nullptr) {
                        delete[] key_buf;
                        return false;
                    }
                    *flag_name = parser.flags[j].flag_name;
                    flag_index = j;
                    break;
                }
            }
        }
    }

    if (flag_index == -1 && arg_index == -1) {
        delete[] key_buf;
        return false;
    }

    if (flag_index != -1) {
        *(parser.flags[flag_index].flag_value) = true;
        delete[] key_buf;
        return true;
    }

    if (arg_index != -1) {
        Argument* arg = &(parser.arguments[arg_index]);
        const char* str_value_arg = nullptr;
        if (arg_val != nullptr) {
            str_value_arg = arg_val;
        } else {
            if (i + 1 >= argc) {
                delete[] key_buf;
                return false;
            }
            str_value_arg = argv[i + 1];
        }

        if (str_value_arg == nullptr) {
            delete[] key_buf;
            return false;
        }

        if (arg->type == ARG_INT) {
            if (!CheckInt(str_value_arg)) {
                delete[] key_buf;
                return false;
            }
            long long_val = std::strtol(str_value_arg, nullptr, 10);
            int curr = static_cast<int>(long_val);
            if (!ArgToParser(parser, arg, curr)) {
                delete[] key_buf;
                return false;
            }
        } else if (arg->type == ARG_FLOAT) {
            if (!CheckFloat(str_value_arg)) {
                delete[] key_buf;
                return false;
            }
            float curr = std::strtof(str_value_arg, nullptr);
            if (!ArgToParser(parser, arg, curr)) {
                delete[] key_buf;
                return false;
            }
        } else if (arg->type == ARG_CHAR) {
            if (!ArgToParser(parser, arg, str_value_arg)) {
                delete[] key_buf;
                return false;
            }
        }

        if (equal_pos == nullptr && arg_val == nullptr) {
            i++;
        }

        delete[] key_buf;
        return true;
    }

    delete[] key_buf;
    return false;
}

int GetRepeatedCount(const ArgumentParser& parser, const char* arg_name) {
    for (int i = 0; i < parser.arguments_count; i++) {
        if (arg_name && parser.arguments[i].arg_name && strcmp(arg_name, parser.arguments[i].arg_name) == 0) {
            return parser.arguments[i].repeated_count;
        }
    }
    return 0;
}

bool GetRepeated(ArgumentParser& parser, const char* arg_name, int n, int* value) {
    for (int i = 0; i < parser.arguments_count; i++) {
        if (arg_name && parser.arguments[i].arg_name && strcmp(arg_name, parser.arguments[i].arg_name) == 0 && parser.arguments[i].type == ARG_INT) {
            if (n < 0 || n >= parser.arguments[i].repeated_count) {
                return false;
            }
            if (n == 0) {
                *value = *(parser.arguments[i].arg_value.int_value);
            } else {
                *value = parser.arguments[i].repeated_arr.int_arr[n - 1];
            }
            return true;
        }
    }
    return false;
}

bool GetRepeated(ArgumentParser& parser, const char* arg_name, int n, float* value) {
    for (int i = 0; i < parser.arguments_count; i++) {
        if (arg_name && parser.arguments[i].arg_name && strcmp(arg_name, parser.arguments[i].arg_name) == 0 && parser.arguments[i].type == ARG_FLOAT) {
            if (n < 0 || n >= parser.arguments[i].repeated_count) {
                return false;
            }
            if (n == 0) {
                *value = *(parser.arguments[i].arg_value.float_value);
            } else {
                *value = parser.arguments[i].repeated_arr.float_arr[n - 1];
            }
            return true;
        }
    }
    return false;
}

bool GetRepeated(ArgumentParser& parser, const char* arg_name, int n, char** value) {
    for (int i = 0; i < parser.arguments_count; i++) {
        if (arg_name && parser.arguments[i].arg_name && strcmp(arg_name, parser.arguments[i].arg_name) == 0 && parser.arguments[i].type == ARG_CHAR) {
            if (n < 0 || n >= parser.arguments[i].repeated_count) {
                return false;
            }
            if (n == 0) {
                *value = parser.arguments[i].arg_value.char_value;
            } else {
                *value = parser.arguments[i].repeated_arr.char_arr[n - 1];
            }
            return true;
        }
    }
    return false;
}

bool GetRepeated(ArgumentParser& parser, const char* arg_name, int n, void* value) {
    char** out = static_cast<char**>(value);
    return GetRepeated(parser, arg_name, n, out);
}

void AddHelp(ArgumentParser& parser) {
    static bool help_requested = false;
    AddFlag(parser, "--help", &help_requested, "Show help message and exit", false);
}

void PrintHelp(const ArgumentParser& parser) {
    if (parser.parser_name == nullptr) {
        std::cout << "Usage: <program> [options]\n";
    } else {
        std::cout << "Usage: " << parser.parser_name << " [options]\n";
    }
    std::cout << "\n";

    if (parser.flags_count > 0) {
        std::cout << "Flags:\n";
        for (int i = 0; i < parser.flags_count; ++i) {
            const Flag& f = parser.flags[i];
            std::cout << "  ";
            if (f.flag_short) {
                std::cout << f.flag_short << ", ";
            }
            std::cout << f.flag_long;
            if (f.flag_name) {
                std::cout << "\t" << f.flag_name;
            }
            std::cout << "\n";
        }
        std::cout << "\n";
    }

    if (parser.arguments_count > 0) {
        std::cout << "Arguments:\n";
        for (int i = 0; i < parser.arguments_count; ++i) {
            const Argument& a = parser.arguments[i];
            std::cout << "  ";
            bool has_name = false;
            if (a.arg_short) {
                std::cout << a.arg_short;
                has_name = true;
            }
            if (a.arg_long) {
                if (has_name) {
                    std::cout << ", ";
                }
                std::cout << a.arg_long;
                has_name = true;
            }
            if (has_name) {
                std::cout << "\t";
            }
            if (a.arg_name) {
                std::cout << a.arg_name;
            }
            if (a.arg_condition) {
                std::cout << " (" << a.arg_condition << ")";
            }
            std::cout << "\n";
        }
        std::cout << "\n";
    }
}

bool Parse(ArgumentParser& parser, int argc, const char** argv) {
    if (argc <= 0 || argv == nullptr) {
        return false;
    }

    for (int i = 0; i < parser.arguments_count; ++i) {
        parser.arguments[i].repeated_count = 0;
    }

    const char** positional_args = new const char*[argc];
    int positional_count = 0;

    for (int i = 1; i < argc; ++i) {
        const char* curr_arg = argv[i];
        if (curr_arg[0] == '-' && !(CheckInt(curr_arg + 1) || CheckFloat(curr_arg + 1))) {
            const char* arg_name = nullptr;
            const char* flag_name = nullptr;
            if (!nargparse::ArgProccess(parser, argc, argv, &arg_name, &flag_name, curr_arg, i)) {
                delete[] positional_args;
                return false;
            }
        } else {
            positional_args[positional_count++] = curr_arg;
        }
    }

    Argument* positional_targets[128];
    int targets_count = 0;
    for (int i = 0; i < parser.arguments_count; ++i) {
        if (parser.arguments[i].arg_short == nullptr && parser.arguments[i].arg_long == nullptr) {
            positional_targets[targets_count++] = &parser.arguments[i];
        }
    }

    if (positional_count > 0 && targets_count == 0) {
        delete[] positional_args;
        return false;
    }

    int pos_idx = 0;
    for (int t = 0; t < targets_count; ++t) {
        Argument* arg = positional_targets[t];
        int take_count = 0;

        if (arg->kNargs == nargparse::kNargsZeroOrMore) {
            take_count = positional_count - pos_idx;
        } else if (arg->kNargs == nargparse::kNargsOneOrMore) {
            if (positional_count - pos_idx == 0) {
                delete[] positional_args;
                return false;
            }
            take_count = positional_count - pos_idx;
        } else if (arg->kNargs == nargparse::kNargsOptional) {
            take_count = (pos_idx < positional_count) ? 1 : 0;
        } else {
            if (pos_idx >= positional_count) {
                delete[] positional_args;
                return false;
            }
            take_count = 1;
        }

        for (int n = 0; n < take_count; ++n) {
            const char* str_val = positional_args[pos_idx++];
            if (arg->type == ARG_INT) {
                if (!CheckInt(str_val)) {
                    delete[] positional_args;
                    return false;
                }
                long long_val = std::strtol(str_val, nullptr, 10);
                int curr = static_cast<int>(long_val);
                if (!ArgToParser(parser, arg, curr)) {
                    delete[] positional_args;
                    return false;
                }
            } else if (arg->type == ARG_FLOAT) {
                if (!CheckFloat(str_val)) {
                    delete[] positional_args;
                    return false;
                }
                float curr = std::strtof(str_val, nullptr);
                if (!ArgToParser(parser, arg, curr)) {
                    delete[] positional_args;
                    return false;
                }
            } else if (arg->type == ARG_CHAR) {
                if (!ArgToParser(parser, arg, str_val)) {
                    delete[] positional_args;
                    return false;
                }
            }
        }
    }

    delete[] positional_args;

    if (pos_idx != positional_count) {
        return false;
    }

    for (int i = 0; i < parser.arguments_count; ++i) {
        const Argument& arg = parser.arguments[i];
        if (arg.kNargs == nargparse::kNargsRequired && arg.repeated_count == 0) {
            return false;
        }
        if (arg.kNargs == nargparse::kNargsOptional && arg.repeated_count > 1) {
            return false;
        }
        if (arg.kNargs == nargparse::kNargsOneOrMore && arg.repeated_count == 0) {
            return false;
        }
    }

    return true;
}

}