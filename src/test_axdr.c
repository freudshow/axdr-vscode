#include "axdr.h"
#include <stdio.h>
#include <string.h>
#include <time.h>

// 测试用例

void test_integer() {
    printf("Testing Integer Encoding/Decoding...\n");
    
    uint8_t buffer[128];
    AXDR_CODEC* codec = axdr_codec_init(buffer, sizeof(buffer));
    
    // 测试整数编码
    int32_t test_values[] = {0, 1, -1, 42, -42, INT32_MAX, INT32_MIN};
    size_t test_count = sizeof(test_values) / sizeof(test_values[0]);
    
    for (size_t i = 0; i < test_count; i++) {
        codec->position = 0;
        int result = axdr_encode_integer(codec, test_values[i], INT32_MIN, INT32_MAX);
        if (result != AXDR_SUCCESS) {
            printf("Failed to encode integer %d: error %d\n", test_values[i], result);
            continue;
        }
        
        int32_t decoded_value;
        codec->position = 0;
        result = axdr_decode_integer(codec, &decoded_value, INT32_MIN, INT32_MAX);
        if (result != AXDR_SUCCESS) {
            printf("Failed to decode integer: error %d\n", result);
            continue;
        }
        
        if (decoded_value != test_values[i]) {
            printf("Integer test failed: expected %d, got %d\n", test_values[i], decoded_value);
        } else {
            printf("Integer test passed: value %d\n", test_values[i]);
        }
    }
    
    axdr_codec_cleanup(codec);
}

void test_boolean() {
    printf("\nTesting Boolean Encoding/Decoding...\n");
    
    uint8_t buffer[128];
    AXDR_CODEC* codec = axdr_codec_init(buffer, sizeof(buffer));
    
    bool test_values[] = {true, false};
    size_t test_count = sizeof(test_values) / sizeof(test_values[0]);
    
    for (size_t i = 0; i < test_count; i++) {
        codec->position = 0;
        int result = axdr_encode_boolean(codec, test_values[i]);
        if (result != AXDR_SUCCESS) {
            printf("Failed to encode boolean %d: error %d\n", test_values[i], result);
            continue;
        }
        
        bool decoded_value;
        codec->position = 0;
        result = axdr_decode_boolean(codec, &decoded_value);
        if (result != AXDR_SUCCESS) {
            printf("Failed to decode boolean: error %d\n", result);
            continue;
        }
        
        if (decoded_value != test_values[i]) {
            printf("Boolean test failed: expected %d, got %d\n", test_values[i], decoded_value);
        } else {
            printf("Boolean test passed: value %d\n", test_values[i]);
        }
    }
    
    axdr_codec_cleanup(codec);
}

void test_enum() {
    printf("\nTesting Enum Encoding/Decoding...\n");
    
    uint8_t buffer[128];
    AXDR_CODEC* codec = axdr_codec_init(buffer, sizeof(buffer));
    
    // 假设枚举有4个值
    int enum_count = 4;
    int test_values[] = {0, 1, 2, 3};
    size_t test_count = sizeof(test_values) / sizeof(test_values[0]);
    
    for (size_t i = 0; i < test_count; i++) {
        codec->position = 0;
        int result = axdr_encode_enum(codec, test_values[i], enum_count);
        if (result != AXDR_SUCCESS) {
            printf("Failed to encode enum %d: error %d\n", test_values[i], result);
            continue;
        }
        
        int decoded_value;
        codec->position = 0;
        result = axdr_decode_enum(codec, &decoded_value, enum_count);
        if (result != AXDR_SUCCESS) {
            printf("Failed to decode enum: error %d\n", result);
            continue;
        }
        
        if (decoded_value != test_values[i]) {
            printf("Enum test failed: expected %d, got %d\n", test_values[i], decoded_value);
        } else {
            printf("Enum test passed: value %d\n", test_values[i]);
        }
    }
    
    axdr_codec_cleanup(codec);
}

void test_bit_string() {
    printf("\nTesting BitString Encoding/Decoding...\n");
    
    uint8_t buffer[128];
    AXDR_CODEC* codec = axdr_codec_init(buffer, sizeof(buffer));
    
    // 测试位串
    uint8_t test_bits[] = {0xA5, 0x5A}; // 10100101 01011010
    size_t bit_length = 16; // 16位
    
    int result = axdr_encode_bit_string(codec, test_bits, bit_length);
    if (result != AXDR_SUCCESS) {
        printf("Failed to encode bit string: error %d\n", result);
        return;
    }
    
    uint8_t decoded_bits[2];
    size_t decoded_length;
    codec->position = 0;
    result = axdr_decode_bit_string(codec, decoded_bits, &decoded_length);
    if (result != AXDR_SUCCESS) {
        printf("Failed to decode bit string: error %d\n", result);
        return;
    }
    
    if (decoded_length != bit_length || 
        decoded_bits[0] != test_bits[0] || 
        decoded_bits[1] != test_bits[1]) {
        printf("BitString test failed\n");
    } else {
        printf("BitString test passed\n");
    }
    
    axdr_codec_cleanup(codec);
}

void test_visible_string() {
    printf("\nTesting VisibleString Encoding/Decoding...\n");
    
    uint8_t buffer[128];
    AXDR_CODEC* codec = axdr_codec_init(buffer, sizeof(buffer));
    
    const char* test_str = "Hello, A-XDR!";
    size_t max_length = 20;
    
    int result = axdr_encode_visible_string(codec, test_str, max_length);
    if (result != AXDR_SUCCESS) {
        printf("Failed to encode visible string: error %d\n", result);
        return;
    }
    
    char decoded_str[21];
    codec->position = 0;
    result = axdr_decode_visible_string(codec, decoded_str, max_length);
    if (result != AXDR_SUCCESS) {
        printf("Failed to decode visible string: error %d\n", result);
        return;
    }
    
    if (strcmp(decoded_str, test_str) != 0) {
        printf("VisibleString test failed: expected '%s', got '%s'\n", test_str, decoded_str);
    } else {
        printf("VisibleString test passed\n");
    }
    
    axdr_codec_cleanup(codec);
}

void test_generalized_time() {
    printf("\nTesting GeneralizedTime Encoding/Decoding...\n");
    
    uint8_t buffer[128];
    AXDR_CODEC* codec = axdr_codec_init(buffer, sizeof(buffer));
    
    time_t test_time = time(NULL);
    
    int result = axdr_encode_generalized_time(codec, test_time);
    if (result != AXDR_SUCCESS) {
        printf("Failed to encode generalized time: error %d\n", result);
        return;
    }
    
    time_t decoded_time;
    codec->position = 0;
    result = axdr_decode_generalized_time(codec, &decoded_time);
    if (result != AXDR_SUCCESS) {
        printf("Failed to decode generalized time: error %d\n", result);
        return;
    }
    
    if (decoded_time != test_time) {
        printf("GeneralizedTime test failed: expected %ld, got %ld\n", test_time, decoded_time);
    } else {
        printf("GeneralizedTime test passed\n");
    }
    
    axdr_codec_cleanup(codec);
}

void test_null() {
    printf("\nTesting NULL Encoding/Decoding...\n");
    
    uint8_t buffer[128];
    AXDR_CODEC* codec = axdr_codec_init(buffer, sizeof(buffer));
    
    int result = axdr_encode_null(codec);
    if (result != AXDR_SUCCESS) {
        printf("Failed to encode NULL: error %d\n", result);
        return;
    }
    
    codec->position = 0;
    result = axdr_decode_null(codec);
    if (result != AXDR_SUCCESS) {
        printf("Failed to decode NULL: error %d\n", result);
        return;
    }
    
    printf("NULL test passed\n");
    
    axdr_codec_cleanup(codec);
}

#include "test_sequence.h"

int main() {
    test_integer();
    test_boolean();
    test_enum();
    test_bit_string();
    test_visible_string();
    test_generalized_time();
    test_null();
    test_sequence();
    test_sequence_of();
    
    return 0;
}