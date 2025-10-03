#include <iostream>
#include <cstring>
#include <cstdio>

void RemoveSubstring(char* str, int start, int length) {
    int len = strlen(str);
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
        }
        if (strcmp(str, "-d") == 0) {
            if (i + 1 >= argc) {
                res.error = false;
                return res;
            }
            res.data_path = argv[i + 1];
        }
        if (strcmp(str, "-o") == 0) {
            if (i + 1 >= argc) {
                res.error = false;
                return res;
            }
            res.output_path = argv[i + 1];
        }
        if (strncmp(str, "--template=", 11) == 0) {
            res.template_path = str + 11;
        }
        if (strncmp(str, "--data=", 7) == 0) {
            res.data_path = str + 7;
        }
        if (strncmp(str, "--output=", 9) == 0) {
            res.output_path = str + 9;
        }
    }
    return res;
}

struct KeysValue {
    char** keys;
    char** value;
    int keys_count;
};

char* DeleteSpace(char* str, int* n, bool flag){
    int j = 0;
    for (int i = 0; i < *n; i++) {
        if (str[i] == '\n' && str[i + 1] == '\n'){
            continue;
        }
        if (str[i] != ' ') {
            str[j] = str[i];
            j++;
        } else if (i != 0 && i != *n - 1){
            if (str[i - 1] != ' ' && str[i + 1] != ' ' && str[i + 1] != '\n' && str[i - 1] != '\n'){
                if (flag == false){
                    str[j] = str[i];
                    j++;
                } else if (str[i + 1] != '=' && str[i - 1] != '=') {
                    return nullptr;
                }
            }
        }
        if (flag == true && str[i] == '-' && i != 0 && i != *n - 1 && str[i - 1] != ' ' && str[i - 1] != '=' && str[i - 1] != '\n' && str[i + 1] != ' ' && str[i + 1] != '=' && str[i + 1] != '\n'){
            return nullptr;
        }
    }
    *n = j;
    str[j] = '\0';
    return str;
}

void ReplacePart(char* str, size_t pos, size_t len, char* replacement) {
    size_t str_len = std::strlen(str);
    size_t rep_len = std::strlen(replacement);

    if (pos > str_len) return;

    if (rep_len > len) {
        for (size_t i = str_len + 1; i > pos + len; --i) {
            str[i + rep_len - len - 1] = str[i - 1];
        }
    }
    else if (rep_len < len) {
        for (size_t i = pos + len; i <= str_len; ++i) {
            str[i - (len - rep_len)] = str[i];
        }
    }

    for (size_t i = 0; i < rep_len; ++i) {
        str[pos + i] = replacement[i];
    }
}


KeysValue DataProcessing(char* data_path){
    KeysValue res; 
    FILE* data_in = fopen(data_path, "r");

    char* data = new char[256];
    int block_size = 256;
    int total_size = 256;
    int symbol_count = 0;
    while(true) {
        if (symbol_count + block_size > total_size) {
            int new_total_size = total_size * 2;
            char* new_data = new char[new_total_size];
            std::memcpy(new_data, data, symbol_count);
            delete[] data;
            data = new_data;
            total_size = new_total_size;
        }
        int curr = std::fread(data + symbol_count, 1, block_size, data_in);
        if (curr == 0) {
            break;
        }
        symbol_count += curr;
    }
    char* new_data = new char[symbol_count];
    std::memcpy(new_data, data, symbol_count);
    delete[] data;
    data = new_data;

    int curr_comment = 0;
    for (int i = 0; i < symbol_count; i++){
        if (data[i] == '/' && data[i + 1] == '/'){
            curr_comment = i;
            int j = i;
            while (data[j] != '\n' && j != symbol_count){
                j++;
            }
            RemoveSubstring(data, curr_comment, j - curr_comment + 1);
            symbol_count -= j - curr_comment + 1;
        }

        if (data[i] == '#'){
            curr_comment = i;
            int j = i;
            while (data[j] != '\n' && j != symbol_count){
                j++;
            }
            RemoveSubstring(data, curr_comment, j - curr_comment + 1);
            symbol_count -= j - curr_comment + 1;
        }
    }

    char* tmp = DeleteSpace(data, &symbol_count, true);
    if (tmp == nullptr) {
        return {nullptr, nullptr, 0};
    }
    data = tmp;

    bool equal_flag = false;
    for (int i = 0; i < symbol_count; i++){
        if (data[i] == '='){
            equal_flag = true;
        }
        if (data[i] == '\n') {
            if (equal_flag == false){
                return {nullptr, nullptr, 0};
            } else {
                equal_flag = false;
                continue;
            }
        }
    }


    int key_count = 0;
    for (int i = 0; i < symbol_count; i++){
        if (data[i] == '=') {
            key_count++;
        }
    }
    char** keys = new char*[key_count];
    char** value = new char*[key_count];
    int last = 0;
    int curr_key = 0;

    for (int i = 0; i < symbol_count; i++) {
        if (data[i] == '=') {
            int len = i - last;
            keys[curr_key] = new char[len + 1];
            std::memcpy(keys[curr_key], data + last, len);
            keys[curr_key][len] = '\0';
            last = i + 1;
        } else if (data[i] == '\n') {
            int len = i - last;
            value[curr_key] = new char[len + 1];
            std::memcpy(value[curr_key], data + last, len);
            value[curr_key][len] = '\0';
            last = i + 1;
            curr_key++;
        }
    }
    if (curr_key < key_count && last < symbol_count) {
        int len = symbol_count - last;
        value[curr_key] = new char[len + 1];
        std::memcpy(value[curr_key], data + last, len);
        value[curr_key][len] = '\0';
    }
    res.keys = keys;
    res.value = value;
    res.keys_count = key_count;
    return res;

}

int TemplateProcessing(KeysValue kv, char* template_path, char* output_path){

    FILE* template_in = fopen(template_path, "r");

    char* tmp = new char[256];
    int block_size = 256;
    int total_size = 256;
    int symbol_count = 0;
    while(true) {
        if (symbol_count + block_size > total_size) {
            int new_total_size = total_size * 2;
            char* new_tmp = new char[new_total_size];
            std::memcpy(new_tmp, tmp, symbol_count);
            delete[] tmp;
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
    char* new_tmp = new char[symbol_count];
    std::memcpy(new_tmp, tmp, symbol_count);
    delete[] tmp;
    tmp = new_tmp;

    int start;
    int end;
    for (int i = 0; i < symbol_count - 1; i++) {
        if (tmp[i] == '{' && tmp[i + 1] == '{') {
            start = i;
            end = -1;
            for (int j = i + 2; j < symbol_count; j++) {
                if ((tmp[j] == '{' && tmp[j + 1] == '{') || (j == symbol_count - 1)) {
                    return 4;
                }
                if (j == symbol_count - 1){
                    break;
                }
                if (tmp[j] == '}' && tmp[j + 1] == '}') {
                    end = j + 1;
                    int key_length = end - start - 3;
                    char* curr_key = new char[key_length + 1];
                    std::memcpy(curr_key, tmp + start + 2, key_length);
                    curr_key[key_length] = '\0';

                    int p = key_length;
                    char* temp = DeleteSpace(curr_key, &p, false);
                    if (temp == nullptr) {
                        return 4;
                    }
                    curr_key = temp;

                    int curr_key_index = -1;
                    for (int k = 0; k < kv.keys_count; k++) {
                        if (strcmp(kv.keys[k], curr_key) == 0) {
                            curr_key_index = k;
                        }
                    }
                    delete[] curr_key;

                    if (curr_key_index == -1) {
                        i = end;
                        break;
                    }

                    int old_len = end - start + 1;
                    const char* replacement = kv.value[curr_key_index];
                    ReplacePart(tmp, start, old_len, const_cast<char*>(replacement));

                    int rep_len = std::strlen(replacement);
                    symbol_count = symbol_count - old_len + rep_len;

                    i = start + rep_len - 1;

                    break;
                }
            }
        }
    }
    if (output_path != nullptr) {
        FILE* file = fopen(output_path, "w");
        if (file == nullptr) {
            return 3;
        }
        fwrite(tmp, sizeof(char), symbol_count, file);
        fclose(file);
    } else {
        for (int i = 0; i < symbol_count; i++){
            std::cout << tmp[i];
        }
    }
    return 0;
}

int main(int argc, char* argv[]) {
    
    Param parameters = Parser(argc, argv);
    if (parameters.template_path == nullptr || parameters.data_path == nullptr || parameters.error == false) {
        std::cout << "Error in arguments" << std::endl;
        return 2;
    }
    KeysValue kv = DataProcessing(parameters.data_path);
    if (kv.keys == nullptr || kv.value == nullptr) {
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
    return 0;
}