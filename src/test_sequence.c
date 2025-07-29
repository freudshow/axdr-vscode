// 测试SEQUENCE
#include "axdr.h"
#include <stdio.h>
#include <string.h>

typedef struct {
    int32_t id;
    bool    active;
    char    name[32];
} TestSequence;

static int encode_sequence_field(AXDR_CODEC* codec, const void* field, int field_type) {
    if (!codec || !field) {
        return AXDR_ERROR_INVALID_VALUE;
    }
    switch (field_type) {
        case 0: // integer
            return axdr_encode_integer(codec, *(const int32_t*)field, INT32_MIN, INT32_MAX);
        case 1: // boolean
            return axdr_encode_boolean(codec, *(const bool*)field);
        case 2: // string
            return axdr_encode_visible_string(codec, (const char*)field, 32);
        default:
            return AXDR_ERROR_INVALID_TYPE;
    }
}

static int decode_sequence_field(AXDR_CODEC* codec, const void* field, int field_type) {
    if (!codec || !field) {
        return AXDR_ERROR_INVALID_VALUE;
    }
    switch (field_type) {
        case 0: // integer
            return axdr_decode_integer(codec, (int32_t*)field, INT32_MIN, INT32_MAX);
        case 1: // boolean
            return axdr_decode_boolean(codec, (bool*)field);
        case 2: // string
            return axdr_decode_visible_string(codec, (char*)field, 32);
        default:
            return AXDR_ERROR_INVALID_TYPE;
    }
}

// 适配 SEQUENCE OF int32_t
static int encode_int32_field(AXDR_CODEC* codec, const void* field) {
    return axdr_encode_integer(codec, *(const int32_t*)field, INT32_MIN, INT32_MAX);
}
static int decode_int32_field(AXDR_CODEC* codec, void* field) {
    return axdr_decode_integer(codec, (int32_t*)field, INT32_MIN, INT32_MAX);
}

void test_sequence() {
    printf("\nTesting SEQUENCE Encoding/Decoding...\n");
    
    uint8_t buffer[256];
    AXDR_CODEC* codec = axdr_codec_init(buffer, sizeof(buffer));
    
    // 准备测试数据
    TestSequence test_data = {
        .id = 12345,
        .active = true
    };
    strncpy(test_data.name, "Test Name", sizeof(test_data.name) - 1);

    // 准备编码参数
    AXDR_ENCODE_PARAMS params[] = {
        {&test_data.id, 0},      // integer
        {&test_data.active, 1},  // boolean
        {test_data.name, 2}      // string
    };
    
    // 编码序列
    int result = axdr_encode_sequence_with_params(codec, params, sizeof(params)/sizeof(params[0]), encode_sequence_field);
    if (result != AXDR_SUCCESS) {
        printf("Failed to encode sequence: error %d\n", result);
        return;
    }
    
    // 准备解码
    TestSequence decoded_data = {0};
    AXDR_ENCODE_PARAMS decode_params[] = {
        {&decoded_data.id, 0},      // integer
        {&decoded_data.active, 1},  // boolean
        {decoded_data.name, 2}      // string
    };
    
    // 解码
    codec->position = 0;
    result = axdr_decode_sequence_with_params(codec, decode_params, sizeof(decode_params)/sizeof(decode_params[0]), decode_sequence_field);
    if (result != AXDR_SUCCESS) {
        printf("Failed to decode sequence: error %d\n", result);
        return;
    }
    
    // 验证结果
    if (decoded_data.id != test_data.id ||
        decoded_data.active != test_data.active ||
        strcmp(decoded_data.name, test_data.name) != 0) {
        printf("SEQUENCE test failed\n");
    } else {
        printf("SEQUENCE test passed\n");
    }
    
    axdr_codec_cleanup(codec);
}

// 测试SEQUENCE OF
void test_sequence_of() {
    printf("\nTesting SEQUENCE OF Encoding/Decoding...\n");
    
    uint8_t buffer[512];
    AXDR_CODEC* codec = axdr_codec_init(buffer, sizeof(buffer));
    
    // 准备测试数据
    int32_t test_array[] = {1, 2, 3, 4, 5};
    AXDR_SEQUENCE_OF test_sequence = {
        .elements = test_array,
        .elementSize = sizeof(int32_t),
        .count = 5,
        .maxCount = 10,
    };
    
    // 编码
    int result = axdr_encode_sequence_of(codec, &test_sequence, encode_int32_field);
    if (result != AXDR_SUCCESS) {
        printf("Failed to encode sequence of: error %d\n", result);
        return;
    }

    // 准备解码
    int32_t decoded_array[10] = {0};
    AXDR_SEQUENCE_OF decoded_sequence = {
        .elements = decoded_array,
        .elementSize = sizeof(int32_t),
        .count = 0,
        .maxCount = 10
    };

    // 解码
    codec->position = 0;
    result = axdr_decode_sequence_of(codec, &decoded_sequence, decode_int32_field);
    if (result != AXDR_SUCCESS) {
        printf("Failed to decode sequence of: error %d\n", result);
        return;
    }

    // 验证结果
    if (decoded_sequence.count != test_sequence.count ||
        memcmp(decoded_array, test_array, sizeof(int32_t) * test_sequence.count) != 0) {
        printf("SEQUENCE OF test failed\n");
    } else {
        printf("SEQUENCE OF test passed\n");
    }

    axdr_codec_cleanup(codec);
}
