#include "number.h"



bool CheckZero(int2025_t num){
    for (int i = 1; i < kBytes; i++){
        if (num.bin_int[i] != 0){
            return false;
        }
    }

    return true;
}

bool Large(const int2025_t& lhs, const int2025_t& rhs){
    if (lhs == rhs) {
        return false;
    }
    for (int i = 0; i < kBytes; i++){
        if (lhs.bin_int[i] > rhs.bin_int[i])
            return true;
        else if (lhs.bin_int[i] < rhs.bin_int[i])
            return false;
    }
    return false;
}

int2025_t Overflow(int2025_t num){
    num.bin_int[0] &= 129;
    int sign = num.bin_int[0] & 128;
    int2025_t num2024;
    num2024.bin_int[0] = 1;
    for (int i = 1; i < kBytes; i++){
        num2024.bin_int[i] = 0;
    }

    if (num == num2024) {
        num.bin_int[0] = 129;
        return num;
    }
    num2024.bin_int[0] = 129;
    if (num == num2024){
        return num;
    }

    int2025_t num_abs = num;
    num_abs.bin_int[0] &= 127;

    int2025_t lb;
    int2025_t rb;
    int2025_t num2025;

    for(int i = 0; i < kBytes; i++){
        lb.bin_int[i] = 0;
        rb.bin_int[i] = 0;
        num2025.bin_int[i] = 255;
    }
    lb.bin_int[0] = 1;
    rb.bin_int[0] = 10;
    num2025.bin_int[0] = 1;
    if (Large(num_abs, lb)){
        num_abs = num_abs - num2025;
        num_abs = num_abs - from_int(1);
        if (sign == 128) {
            num_abs.bin_int[0] &= 127;
        } else {
            num_abs.bin_int[0] |= 128;
        }

        return num_abs;
    }
    if(CheckZero(num) && ((num.bin_int[0] & 127) == 0)){
        num.bin_int[0] = 0;
    }
    
    return num;
}

bool is_negative(const int2025_t& a) {
    return (a.bin_int[0] & 128) != 0;
}

int2025_t MultiplyInt(const int2025_t &a, const int2025_t &b) {
    int2025_t result;
    for(int i = 0; i < kBytes; i++){
        result.bin_int[i] = 0;
    }

    int2025_t a_abs = a;
    a_abs.bin_int[0] &= 127;
    int2025_t b_abs = b;
    b_abs.bin_int[0] &= 127;

    for (int i = kBytes - 1; i >= 0; i--) {
        int carry = 0;
        int av = a_abs.bin_int[i];
        for (int j = kBytes - 1; j >= 0; j--) {
            int k = i + j - (kBytes - 1);
            if (k < 0 || k >= kBytes) {
                continue;
            }

            int bv = b_abs.bin_int[j];
            int product = av * bv + result.bin_int[k] + carry;
            result.bin_int[k] = product & 255;
            carry = product >> 8;
        }
        int k = i - 1;
        while (carry > 0 && k >= 0) {
            int sum = result.bin_int[k] + carry;
            result.bin_int[k] = sum & 255;
            carry = sum >> 8;
            k--;
        }
    }

    bool sign_result = is_negative(a) ^ is_negative(b);
    if (sign_result) {
        result.bin_int[0] |= 128;
    } else {
        result.bin_int[0] &= 127;
    }

    return result;
}

int2025_t from_int(int32_t i) {
    int2025_t num;
    int64_t n = i;
    for (int j = 0; j < kBytes; j++){
        num.bin_int[j] = 0;
    }
    bool sign = false;
    if (n < 0){
        n = -n;
        sign = true;
    }

    if (n != 0){
        for(int j = 0; j < ((int)log2(n))/8 + 1; j++){
            num.bin_int[253 - j] = (n >> 8*j) & 255;
        }
    }

    if (sign){
        if ((int)num.bin_int[0] == 1){
            num.bin_int[0] = 129;
        } else {
            num.bin_int[0] = 128;
        }
    }

    return num;
}

int2025_t from_string(const char* buff) {
    int2025_t res;
    int2025_t pow10;
    int sign = 0;
    if (buff[0] == '+'){
        buff++;
    }
    if (buff[0] == '-'){
        sign = 128;
        buff++;
    } else {
        sign = 0;
    }

    for(int i = 0; i < kBytes; i++){
        res.bin_int[i] = 0;
        pow10.bin_int[i] = 0;
    }
    pow10.bin_int[253] = 1;

    int len = std::strlen(buff);
    char curr;
    for(int i = len - 1; i >= 0; i--){
        curr = buff[i];
        res = res + (from_int(curr - '0') * pow10);
        pow10 = pow10 * from_int(10);
    }
    if(sign == 128) {
        res.bin_int[0] |= 128;
    } else {
        res.bin_int[0] &= 127;
    }

    return Overflow(res);
}

int2025_t SumOfAbs(const int2025_t& lhs, const int2025_t& rhs){
    int2025_t res;
    for(int i = 0; i < kBytes; i++){
        res.bin_int[i] = 0;
    }
    int rem = 0;
    int curr_sum = 0;
    for(int i = 253; i >= 0; i--){
        curr_sum = rem + lhs.bin_int[i] + rhs.bin_int[i];
        rem = 0;
        while(curr_sum > 255){
            rem++;
            curr_sum -= 256;
        }
        res.bin_int[i] = curr_sum;
    }
    return Overflow(res);
}

int2025_t DifOfAbs(const int2025_t& l, const int2025_t& r){
    int2025_t res;
    for(int i = 0; i < kBytes; i++){
        res.bin_int[i] = 0;
    }
    int2025_t lhs = l;
    int2025_t rhs = r;

    int curr_dif = 0;
    for(int i = 253; i >= 0; i--){
        curr_dif = lhs.bin_int[i] - rhs.bin_int[i];
        int k = i - 1;
        while(curr_dif < 0){
            while(lhs.bin_int[k] == 0){
                lhs.bin_int[k] = 255;
                k--;
            }
            if(k < 0){
                break;
            }
            lhs.bin_int[k]--;
            curr_dif += 256;
        }
        res.bin_int[i] = curr_dif;
    }
    res.bin_int[0] &= 127;
    return Overflow(res);
}

int Rem10(int2025_t& num) {
    int rem = 0;
    for (int i = 0; i < kBytes; i++) {
        int curr = (rem << 8) + num.bin_int[i];
        num.bin_int[i] = curr / 10;
        rem = curr % 10;
    }
    return rem;
}

char* IntToStr(int2025_t num){
    if (CheckZero(num) && ((num.bin_int[0] & 127) == 0)){
        char* zero = (char*)malloc(2);
        zero[0] = '0';
        zero[1] = '\0';
        
        return zero;
    }

    int sign = num.bin_int[0] & 128;
    num.bin_int[0] = num.bin_int[0] & 127;

    char* str = (char*)malloc(650);

    int i = 0;
    while((!CheckZero(num) || num.bin_int[0] != 0) && i < 650){
        int rem = Rem10(num);
        str[i] = rem + '0';
        i++;
    }

    if(sign == 128){
        str[i] = '-';
        i++;
    }
    char* new_str = (char*)malloc(i + 1);
    
    for(int j = 0; j < i; j++){
        new_str[j] = str[i - j - 1];
    }
    free(str);
    new_str[i] = '\0';
    str = new_str;

    return str;
}

int2025_t operator+(const int2025_t& lhs, const int2025_t& rhs) {
    int2025_t res;
    for(int i = 0; i < 254; i++){
        res.bin_int[i] = 0;
    }

    if(!is_negative(lhs) && !is_negative(rhs)){
        return SumOfAbs(lhs, rhs);
    } else if (is_negative(lhs) && is_negative(rhs)){
        int2025_t abs_lhs = lhs;
        int2025_t abs_rhs = rhs;
        abs_lhs.bin_int[0] &= 127;
        abs_rhs.bin_int[0] &= 127;

        res = SumOfAbs(abs_lhs, abs_rhs);
        res.bin_int[0] |= 128;
        return Overflow(res);
    } else {
        int2025_t abs_lhs = lhs;
        int2025_t abs_rhs = rhs;
        abs_lhs.bin_int[0] &= 127;
        abs_rhs.bin_int[0] &= 127;
        if(Large(abs_lhs, abs_rhs) && is_negative(lhs)){
            res = DifOfAbs(lhs, rhs);
            res.bin_int[0] |= 128;
            return Overflow(res);
        } else if (Large(abs_rhs, abs_lhs) && is_negative(rhs)){
            res = DifOfAbs(lhs, rhs);
            res.bin_int[0] |= 128;
            return Overflow(res);
        } else{
            if(Large(abs_lhs, abs_rhs)){
                res = DifOfAbs(abs_lhs, abs_rhs);
                return Overflow(res);
            } else{
                res = DifOfAbs(rhs, lhs);
                return Overflow(res);
            }
        }
    }
}

int2025_t operator-(const int2025_t& lhs, const int2025_t& rhs) {
    int2025_t res;
    int2025_t abs_lhs = lhs;
    int2025_t abs_rhs = rhs;
    abs_lhs.bin_int[0] &= 127;
    abs_rhs.bin_int[0] &= 127;
    if(!(is_negative(lhs) ^ is_negative(rhs))){
        int sign = lhs.bin_int[0] & 128;

        if(Large(abs_lhs, abs_rhs)){
            res = DifOfAbs(abs_lhs, abs_rhs);
        } else {
            res = DifOfAbs(abs_rhs, abs_lhs);
            if (sign == 128){
                sign = 0;
            } else {
                sign = 128;
            }
        }
        res.bin_int[0] |= sign;
        return Overflow(res); 
    } else {
        if(is_negative(lhs)){
            res = SumOfAbs(abs_lhs, abs_rhs);
            res.bin_int[0] |= 128;
            return Overflow(res);
        } else{
            return Overflow(SumOfAbs(abs_lhs, abs_rhs));
        }
    }
}

int2025_t operator*(const int2025_t& lhs, const int2025_t& rhs) {
    int2025_t res = MultiplyInt(lhs, rhs);
    
    return Overflow(res);
}

int2025_t operator/(const int2025_t& lhs, const int2025_t& rhs) {
    if (lhs == from_int(0) || rhs == from_int(0)){
        return from_int(0);
    }
    int2025_t abs_lhs = lhs;
    int2025_t abs_rhs = rhs;
    abs_lhs.bin_int[0] &= 127;
    abs_rhs.bin_int[0] &= 127;

    if (Large(abs_rhs, abs_lhs) && abs_rhs != abs_lhs) {
        return from_int(0);
    }

    int2025_t x = from_int(0);
    int2025_t one = from_int(1);

    while (!Large(abs_rhs, abs_lhs) || abs_rhs == abs_lhs) {
        int2025_t temp_divisor = abs_rhs;
        int2025_t multiple = one;

        while (!Large(temp_divisor + temp_divisor, abs_lhs)) {
            temp_divisor = temp_divisor + temp_divisor;
            multiple = multiple + multiple;
        }

        abs_lhs = abs_lhs - temp_divisor;
        x = x + multiple;
    }

    if (is_negative(lhs) ^ is_negative(rhs)) {
        x.bin_int[0] |= 128; 
    } else {
        x.bin_int[0] &= 127;
    }

    return Overflow(x);
}

bool operator==(const int2025_t& lhs, const int2025_t& rhs) {
    for (int i = 0; i < kBytes; i++){
        if(lhs.bin_int[i] != rhs.bin_int[i]){
            return false;
        }
    }
    return true;
}

bool operator!=(const int2025_t& lhs, const int2025_t& rhs) {
    for (int i = 0; i < kBytes; i++){
        if(lhs.bin_int[i] != rhs.bin_int[i]){
            return true;
        }
    }
    return false;
}

std::ostream& operator<<(std::ostream& stream, const int2025_t& value) {
    char* str = IntToStr(value);
    stream << str;

    free(str);
    return stream;
}