#include <iostream>
#include <cstring>
#include <cstdio>
#include <cstdlib>

void RemoveSubstring(char* str, int start, int length) {
    int len = std::strlen(str);
    if (start < 0 || start > len) return;
    if (length < 0) return;
    if (start + length > len) length = len - start;
    for (int i = start; i <= len - length; ++i) {
        str[i] = str[i + length];
    }
}

struct Param {
    char* template_path;
    char* data_path;
    char* output_path;
    bool error;
};

Param Parser(int argc, char* argv[]) {
    Param res = {nullptr, nullptr, nullptr, true};
    for (int i = 1; i < argc; i++) {
        char* str = argv[i];

        if (strcmp(str, "-t") == 0) {
            if (i + 1 >= argc) {
                res.error = false;
                return res;
            }
            res.template_path = argv[i + 1];
            i++;
            continue;
        }
        if (strcmp(str, "-d") == 0) {
            if (i + 1 >= argc) {
                res.error = false;
                return res;
            }
            res.data_path = argv[i + 1];
            i++;
            continue;
        }
        if (strcmp(str, "-o") == 0) {
            if (i + 1 >= argc) {
                res.error = false;
                return res;
            }
            res.output_path = argv[i + 1];
            i++;
            continue;
        }
        if (strncmp(str, "--template=", 11) == 0) {
            res.template_path = str + 11;
            continue;
        }
        if (strncmp(str, "--data=", 7) == 0) {
            res.data_path = str + 7;
            continue;
        }
        if (strncmp(str, "--output=", 9) == 0) {
            res.output_path = str + 9;
            continue;
        }
    }
    return res;
}

struct KeysValue {
    char** keys;
    char** value;
    int keys_count;
};

char* DeleteSpace(char* str, int* n) {
    int j = 0;
    for (int i = 0; i < *n; i++) {
        if (i + 1 < *n && str[i] == '\n' && str[i + 1] == '\n') {
            continue;
        }
        if (str[i] != ' ') {
            str[j] = str[i];
            j++;
        } else if (i != 0 && i != *n - 1) {
            if (str[i - 1] != ' ' && i + 1 < *n && str[i + 1] != ' ' && str[i + 1] != '\n' && str[i - 1] != '\n') {
                if (str[i + 1] != '=' && str[i - 1] != '=') {
                    return nullptr;
                }
            }
        }
        if (str[i] == '-' && i != 0 && i != *n - 1) {
            if (str[i - 1] != ' ' && str[i - 1] != '=' && str[i - 1] != '\n' && str[i + 1] != ' ' && str[i + 1] != '=' && str[i + 1] != '\n') {
                return nullptr;
            }
        }
    }
    *n = j;
    str[j] = '\0';
    return str;
}

void ReplacePart(char* str, size_t pos, size_t len, const char* replacement) {
    size_t str_len = std::strlen(str);
    size_t rep_len = std::strlen(replacement);

    if (pos > str_len) return;

    if (rep_len > len) {
        for (size_t i = str_len + 1; i > pos + len; --i) {
            str[i + rep_len - len - 1] = str[i - 1];
        }
    } else if (rep_len < len) {
        for (size_t i = pos + len; i <= str_len; ++i) {
            str[i - (len - rep_len)] = str[i];
        }
    }

    for (size_t i = 0; i < rep_len; ++i) {
        str[pos + i] = replacement[i];
    }
}

KeysValue DataProcessing(char* data_path) {
    KeysValue res{nullptr, nullptr, 0};
    FILE* data_in = fopen(data_path, "r");
    if (data_in == nullptr) {
        return res;
    }

    int block_size = 256;
    int total_size = block_size;
    int symbol_count = 0;
    char* data = (char*)malloc(total_size);

    while (true) {
        if (symbol_count + block_size > total_size) {
            int new_total_size = total_size * 2;
            char* new_data = (char*)malloc(new_total_size);
            std::memcpy(new_data, data, symbol_count);
            free(data);
            data = new_data;
            total_size = new_total_size;
        }
        int curr = std::fread(data + symbol_count, 1, block_size, data_in);
        if (curr == 0) {
            break;
        }
        symbol_count += curr;
    }
    fclose(data_in);

    char* new_data = (char*)malloc(symbol_count + 1);
    std::memcpy(new_data, data, symbol_count);
    new_data[symbol_count] = '\0';
    free(data);
    data = new_data;

    int curr_comment = 0;
    for (int i = 0; i < symbol_count; i++) {
        if (i + 1 < symbol_count && data[i] == '/' && data[i + 1] == '/') {
            curr_comment = i;
            int j = i;
            while (j < symbol_count && data[j] != '\n') {
                j++;
            }
            RemoveSubstring(data, curr_comment, j - curr_comment);
            symbol_count -= j - curr_comment;
            i = curr_comment - 1;
            if (i < 0) i = 0;
            continue;
        }

        if (data[i] == '#') {
            curr_comment = i;
            int j = i;
            while (j < symbol_count && data[j] != '\n') {
                j++;
            }
            RemoveSubstring(data, curr_comment, j - curr_comment);
            symbol_count -= j - curr_comment;
            i = curr_comment - 1;
            if (i < 0) i = 0;
            continue;
        }
    }

    char* tmp = DeleteSpace(data, &symbol_count);
    if (tmp == nullptr) {
        free(data);
        return {nullptr, nullptr, 0};
    }
    data = tmp;

    bool equal_flag = false;
    for (int i = 0; i < symbol_count; i++) {
        if (data[i] == '=') {
            equal_flag = true;
        }
        if (data[i] == '\n') {
            if (equal_flag == false) {
                free(data);
                return {nullptr, nullptr, 0};
            } else {
                equal_flag = false;
                continue;
            }
        }
    }

    int key_count = 0;
    for (int i = 0; i < symbol_count; i++) {
        if (data[i] == '=') {
            key_count++;
        }
    }
    char** keys = (char**)malloc(sizeof(char*) * key_count);
    char** value = (char**)malloc(sizeof(char*) * key_count);
    int last = 0;
    int curr_key = 0;

    for (int i = 0; i < symbol_count; i++) {
        if (data[i] == '=') {
            int len = i - last;
            keys[curr_key] = (char*)malloc(len + 1);
            std::memcpy(keys[curr_key], data + last, len);
            keys[curr_key][len] = '\0';
            last = i + 1;
        } else if (data[i] == '\n') {
            int len = i - last;
            value[curr_key] = (char*)malloc(len + 1);
            std::memcpy(value[curr_key], data + last, len);
            value[curr_key][len] = '\0';
            last = i + 1;
            curr_key++;
        }
    }
    if (curr_key < key_count && last < symbol_count) {
        int len = symbol_count - last;
        value[curr_key] = (char*)malloc(len + 1);
        std::memcpy(value[curr_key], data + last, len);
        value[curr_key][len] = '\0';
    }
    free(data);
    res.keys = keys;
    res.value = value;
    res.keys_count = key_count;
    return res;
}

int TemplateProcessing(KeysValue kv, char* template_path, char* output_path) {
    FILE* template_in = fopen(template_path, "r");
    if (template_in == nullptr) return 3;

    int block_size = 256;
    int total_size = block_size;
    int symbol_count = 0;
    char* tmp = (char*)malloc(total_size);

    while (true) {
        if (symbol_count + block_size > total_size) {
            int new_total_size = total_size * 2;
            char* new_tmp = (char*)malloc(new_total_size);
            std::memcpy(new_tmp, tmp, symbol_count);
            free(tmp);
            tmp = new_tmp;
            total_size = new_total_size;
        }
        int curr = std::fread(tmp + symbol_count, 1, block_size, template_in);
        if (curr == 0) {
            break;
        }
        symbol_count += curr;
    }
    fclose(template_in);

    if (symbol_count + 1 > total_size) {
        char* new_tmp = (char*)malloc(symbol_count + 1);
        std::memcpy(new_tmp, tmp, symbol_count);
        free(tmp);
        tmp = new_tmp;
        total_size = symbol_count + 1;
    }
    tmp[symbol_count] = '\0';

    for (int i = 0; i < symbol_count - 1; i++) {
        if (tmp[i] == '{' && tmp[i + 1] == '{') {
            int start_scb = i;
            i += 2;
            while (i < symbol_count && tmp[i] == ' ') {
                i++;
            }
            int start = i;
            int end = -1;
            while (i + 1 < symbol_count) {
                if (tmp[i] == '}' && tmp[i + 1] == '}') {
                    if (end == -1) {
                        end = i - 1;
                    }
                    break;
                }
                if (tmp[i] == ' ' && end == -1) {
                    end = i - 1;
                }
                i++;
            }
            if (i + 1 >= symbol_count) {
                free(tmp);
                return 4;
            }
            if (end == -1) {
                end = i - 1;
            }
            int end_scb = i + 1;

            int key_len = end - start + 1;
            if (key_len <= 0) {
                free(tmp);
                return 4;
            }

            char* curr_key = (char*)malloc(key_len + 1);
            std::memcpy(curr_key, tmp + start, key_len);
            curr_key[key_len] = '\0';

            int curr_key_index = -1;
            for (int k = 0; k < kv.keys_count; k++) {
                if (strcmp(kv.keys[k], curr_key) == 0) {
                    curr_key_index = k;
                }
            }
            free(curr_key);

            if (curr_key_index == -1) {
                free(tmp);
                return 4;
            }

            int old_len = end_scb - start_scb + 1;
            const char* replacement = kv.value[curr_key_index];
            size_t rep_len = std::strlen(replacement);

            if (rep_len > old_len) {
                if (symbol_count + (rep_len - old_len) + 1 > total_size) {
                    int new_total = total_size;
                    while (new_total < symbol_count + (rep_len - old_len) + 1) {
                        new_total *= 2;
                    }
                    char* new_tmp = (char*)malloc(new_total);
                    std::memcpy(new_tmp, tmp, start_scb);
                    std::memcpy(new_tmp + start_scb + rep_len, tmp + start_scb + old_len, symbol_count - (start_scb + old_len));
                    free(tmp);
                    tmp = new_tmp;
                    total_size = new_total;
                } else {
                    for (int idx = symbol_count; idx >= start_scb + old_len; idx--) {
                        tmp[idx + rep_len - old_len] = tmp[idx];
                    }
                }
            } else if (rep_len < old_len) {
                for (int idx = start_scb + old_len; idx <= symbol_count; idx++) {
                    tmp[idx - (old_len - rep_len)] = tmp[idx];
                }
            }

            for (size_t idx = 0; idx < rep_len; idx++) {
                tmp[start_scb + idx] = replacement[idx];
            }

            symbol_count = symbol_count - old_len + static_cast<int>(rep_len);
            tmp[symbol_count] = '\0';

            i = start_scb + static_cast<int>(rep_len) - 1;
        }
    }

    if (output_path != nullptr) {
        FILE* file = fopen(output_path, "w");
        if (file == nullptr) {
            free(tmp);
            return 3;
        }
        fwrite(tmp, 1, symbol_count, file);
        fclose(file);
    } else {
        std::cout.write(tmp, symbol_count);
    }
    free(tmp);

    return 0;
}

int main(int argc, char* argv[]) {
    Param parameters = Parser(argc, argv);
    if (parameters.template_path == nullptr || parameters.data_path == nullptr || parameters.error == false) {
        std::cout << "Error in arguments" << std::endl;
        return 2;
    }
    KeysValue kv = DataProcessing(parameters.data_path);
    if (kv.keys == nullptr || kv.value == nullptr || kv.keys_count == 0) {
        std::cout << "Error in data file" << std::endl;
        return 5;
    }

    int err = TemplateProcessing(kv, parameters.template_path, parameters.output_path);
    switch (err) {
    case 4:
        std::cout << "Error in template file" << std::endl;
        return 4;
    case 3:
        std::cout << "Error in opening file" << std::endl;
        return 3;
    }

    for (int i = 0; i < kv.keys_count; i++) {
        free(kv.keys[i]);
        free(kv.value[i]);
    }
    free(kv.keys);
    free(kv.value);

    return 0;
}