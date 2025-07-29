#include "axdr.h"
#include <string.h>
#include <stdlib.h>

// 上下文操作函数实现
AXDR_CODEC* axdr_codec_init(uint8_t* buffer, size_t size) {
    AXDR_CODEC* codec = (AXDR_CODEC*)malloc(sizeof(AXDR_CODEC));
    if (codec) {
        codec->buffer = buffer;
        codec->size = size;
        codec->position = 0;
        codec->error = AXDR_SUCCESS;
    }
    return codec;
}

void axdr_codec_cleanup(AXDR_CODEC* codec) {
    free(codec);
}

// 整数编码实现
int axdr_encode_integer(AXDR_CODEC* codec, int32_t value, int32_t min, int32_t max) {
    // 约束检查
    if (value < min || value > max) {
        return AXDR_ERROR_CONSTRAINT;
    }
    
    // 确保缓冲区足够
    if (codec->position + 4 > codec->size) {
        return AXDR_ERROR_BUFFER_OVERFLOW;
    }
    
    // 网络字节序编码（大端序）
    codec->buffer[codec->position++] = (value >> 24) & 0xFF;
    codec->buffer[codec->position++] = (value >> 16) & 0xFF;
    codec->buffer[codec->position++] = (value >> 8) & 0xFF;
    codec->buffer[codec->position++] = value & 0xFF;
    
    return AXDR_SUCCESS;
}

// 无符号整数编码实现
int axdr_encode_unsigned(AXDR_CODEC* codec, uint32_t value, uint32_t max) {
    // 约束检查
    if (value > max) {
        return AXDR_ERROR_CONSTRAINT;
    }
    
    // 确保缓冲区足够
    if (codec->position + 4 > codec->size) {
        return AXDR_ERROR_BUFFER_OVERFLOW;
    }
    
    // 网络字节序编码
    codec->buffer[codec->position++] = (value >> 24) & 0xFF;
    codec->buffer[codec->position++] = (value >> 16) & 0xFF;
    codec->buffer[codec->position++] = (value >> 8) & 0xFF;
    codec->buffer[codec->position++] = value & 0xFF;
    
    return AXDR_SUCCESS;
}

// 布尔值编码实现
int axdr_encode_boolean(AXDR_CODEC* codec, bool value) {
    if (codec->position + 1 > codec->size) {
        return AXDR_ERROR_BUFFER_OVERFLOW;
    }
    
    codec->buffer[codec->position++] = value ? 0xFF : 0x00;
    return AXDR_SUCCESS;
}

// 枚举编码实现
int axdr_encode_enum(AXDR_CODEC* codec, int value, int count) {
    if (value < 0 || value >= count) {
        return AXDR_ERROR_CONSTRAINT;
    }
    
    return axdr_encode_integer(codec, value, 0, count - 1);
}

// 位串编码实现
int axdr_encode_bit_string(AXDR_CODEC* codec, const uint8_t* bits, size_t length) {
    // 编码长度
    if (codec->position + 4 + ((length + 7) / 8) > codec->size) {
        return AXDR_ERROR_BUFFER_OVERFLOW;
    }
    
    axdr_encode_unsigned(codec, length, UINT32_MAX);
    
    // 编码位串内容
    size_t byte_length = (length + 7) / 8;
    memcpy(codec->buffer + codec->position, bits, byte_length);
    codec->position += byte_length;
    
    return AXDR_SUCCESS;
}

// 字节串编码实现
int axdr_encode_octet_string(AXDR_CODEC* codec, const uint8_t* octets, size_t length) {
    // 编码长度
    if (codec->position + 4 + length > codec->size) {
        return AXDR_ERROR_BUFFER_OVERFLOW;
    }
    
    axdr_encode_unsigned(codec, length, UINT32_MAX);
    
    // 编码内容
    memcpy(codec->buffer + codec->position, octets, length);
    codec->position += length;
    
    return AXDR_SUCCESS;
}

// 可视串编码实现
int axdr_encode_visible_string(AXDR_CODEC* codec, const char* str, size_t max_length) {
    size_t length = strlen(str);
    if (length > max_length) {
        return AXDR_ERROR_CONSTRAINT;
    }
    
    return axdr_encode_octet_string(codec, (const uint8_t*)str, length);
}

// 通用时间编码实现
int axdr_encode_generalized_time(AXDR_CODEC* codec, time_t time) {
    struct tm* tm_info = gmtime(&time);
    char time_str[15];
    
    strftime(time_str, sizeof(time_str), "%Y%m%d%H%M%S", tm_info);
    return axdr_encode_visible_string(codec, time_str, 14);
}

// NULL值编码实现
int axdr_encode_null(AXDR_CODEC* codec) {
    // NULL类型不需要编码任何内容
    return AXDR_SUCCESS;
}

// 整数解码实现
int axdr_decode_integer(AXDR_CODEC* codec, int32_t* value, int32_t min, int32_t max) {
    if (codec->position + 4 > codec->size) {
        return AXDR_ERROR_BUFFER_OVERFLOW;
    }
    
    *value = (codec->buffer[codec->position] << 24) |
             (codec->buffer[codec->position + 1] << 16) |
             (codec->buffer[codec->position + 2] << 8) |
             codec->buffer[codec->position + 3];
    codec->position += 4;
    
    if (*value < min || *value > max) {
        return AXDR_ERROR_CONSTRAINT;
    }
    
    return AXDR_SUCCESS;
}

// 无符号整数解码实现
int axdr_decode_unsigned(AXDR_CODEC* codec, uint32_t* value, uint32_t max) {
    if (codec->position + 4 > codec->size) {
        return AXDR_ERROR_BUFFER_OVERFLOW;
    }
    
    *value = ((uint32_t)codec->buffer[codec->position] << 24) |
             ((uint32_t)codec->buffer[codec->position + 1] << 16) |
             ((uint32_t)codec->buffer[codec->position + 2] << 8) |
             (uint32_t)codec->buffer[codec->position + 3];
    codec->position += 4;
    
    if (*value > max) {
        return AXDR_ERROR_CONSTRAINT;
    }
    
    return AXDR_SUCCESS;
}

// 布尔值解码实现
int axdr_decode_boolean(AXDR_CODEC* codec, bool* value) {
    if (codec->position + 1 > codec->size) {
        return AXDR_ERROR_BUFFER_OVERFLOW;
    }
    
    *value = (codec->buffer[codec->position++] != 0);
    return AXDR_SUCCESS;
}

// 枚举解码实现
int axdr_decode_enum(AXDR_CODEC* codec, int* value, int count) {
    int32_t temp;
    int result = axdr_decode_integer(codec, &temp, 0, count - 1);
    if (result == AXDR_SUCCESS) {
        *value = temp;
    }
    return result;
}

// 位串解码实现
int axdr_decode_bit_string(AXDR_CODEC* codec, uint8_t* bits, size_t* length) {
    uint32_t bit_length;
    int result = axdr_decode_unsigned(codec, &bit_length, UINT32_MAX);
    if (result != AXDR_SUCCESS) {
        return result;
    }
    
    *length = bit_length;
    size_t byte_length = (bit_length + 7) / 8;
    
    if (codec->position + byte_length > codec->size) {
        return AXDR_ERROR_BUFFER_OVERFLOW;
    }
    
    memcpy(bits, codec->buffer + codec->position, byte_length);
    codec->position += byte_length;
    
    return AXDR_SUCCESS;
}

// 字节串解码实现
int axdr_decode_octet_string(AXDR_CODEC* codec, uint8_t* octets, size_t* length) {
    uint32_t str_length;
    int result = axdr_decode_unsigned(codec, &str_length, UINT32_MAX);
    if (result != AXDR_SUCCESS) {
        return result;
    }
    
    *length = str_length;
    
    if (codec->position + str_length > codec->size) {
        return AXDR_ERROR_BUFFER_OVERFLOW;
    }
    
    memcpy(octets, codec->buffer + codec->position, str_length);
    codec->position += str_length;
    
    return AXDR_SUCCESS;
}

// 可视串解码实现
int axdr_decode_visible_string(AXDR_CODEC* codec, char* str, size_t max_length) {
    size_t length;
    int result = axdr_decode_octet_string(codec, (uint8_t*)str, &length);
    if (result != AXDR_SUCCESS) {
        return result;
    }
    
    if (length > max_length) {
        return AXDR_ERROR_CONSTRAINT;
    }
    
    str[length] = '\0';
    return AXDR_SUCCESS;
}

// 通用时间解码实现
int axdr_decode_generalized_time(AXDR_CODEC* codec, time_t* time) {
    char time_str[15];
    int result = axdr_decode_visible_string(codec, time_str, 14);
    if (result != AXDR_SUCCESS) {
        return result;
    }
    
    struct tm tm_info = {0};
    if (sscanf(time_str, "%4d%2d%2d%2d%2d%2d",
               &tm_info.tm_year, &tm_info.tm_mon, &tm_info.tm_mday,
               &tm_info.tm_hour, &tm_info.tm_min, &tm_info.tm_sec) != 6) {
        return AXDR_ERROR_INVALID_VALUE;
    }
    
    tm_info.tm_year -= 1900;
    tm_info.tm_mon -= 1;
    
    *time = timegm(&tm_info);
    return AXDR_SUCCESS;
}

// NULL值解码实现
int axdr_decode_null(AXDR_CODEC* codec) {
    // NULL类型不需要解码任何内容
    return AXDR_SUCCESS;
}

// 可变长度整型编码（BER风格，最小字节数）
int axdr_encode_varint(AXDR_CODEC* codec, int32_t value) {
    uint8_t buf[5];
    int len = 0;
    uint32_t uval = (uint32_t)value;
    // ZigZag编码可选，这里直接用无符号
    do {
        buf[len] = uval & 0x7F;
        uval >>= 7;
        if (uval) buf[len] |= 0x80;
        len++;
    } while (uval && len < 5);
    if (codec->position + len > codec->size) return AXDR_ERROR_BUFFER_OVERFLOW;
    memcpy(codec->buffer + codec->position, buf, len);
    codec->position += len;
    return AXDR_SUCCESS;
}

// 可变长度整型解码
int axdr_decode_varint(AXDR_CODEC* codec, int32_t* value) {
    uint32_t result = 0;
    int shift = 0;
    int i = 0;
    while (i < 5) {
        if (codec->position >= codec->size) return AXDR_ERROR_BUFFER_OVERFLOW;
        uint8_t byte = codec->buffer[codec->position++];
        result |= (uint32_t)(byte & 0x7F) << shift;
        if (!(byte & 0x80)) break;
        shift += 7;
        i++;
    }
    *value = (int32_t)result;
    return AXDR_SUCCESS;
}

// 可变长度字节串编码
int axdr_encode_varoctet_string(AXDR_CODEC* codec, const uint8_t* octets, size_t length) {
    int res = axdr_encode_varint(codec, (int32_t)length);
    if (res != AXDR_SUCCESS) return res;
    if (codec->position + length > codec->size) return AXDR_ERROR_BUFFER_OVERFLOW;
    memcpy(codec->buffer + codec->position, octets, length);
    codec->position += length;
    return AXDR_SUCCESS;
}

// 可变长度字节串解码
int axdr_decode_varoctet_string(AXDR_CODEC* codec, uint8_t* octets, size_t* length, size_t max_length) {
    int32_t len = 0;
    int res = axdr_decode_varint(codec, &len);
    if (res != AXDR_SUCCESS) return res;
    if (len < 0 || (size_t)len > max_length) return AXDR_ERROR_CONSTRAINT;
    if (codec->position + len > codec->size) return AXDR_ERROR_BUFFER_OVERFLOW;
    memcpy(octets, codec->buffer + codec->position, len);
    codec->position += len;
    *length = (size_t)len;
    return AXDR_SUCCESS;
}

// 可变长度可视串编码
int axdr_encode_varvisible_string(AXDR_CODEC* codec, const char* str) {
    size_t length = strlen(str);
    return axdr_encode_varoctet_string(codec, (const uint8_t*)str, length);
}

// 可变长度可视串解码
int axdr_decode_varvisible_string(AXDR_CODEC* codec, char* str, size_t* length, size_t max_length) {
    int res = axdr_decode_varoctet_string(codec, (uint8_t*)str, length, max_length);
    if (res == AXDR_SUCCESS) str[*length] = '\0';
    return res;
}

// 可变长度位串编码
int axdr_encode_varbit_string(AXDR_CODEC* codec, const uint8_t* bits, size_t bit_length) {
    int res = axdr_encode_varint(codec, (int32_t)bit_length);
    if (res != AXDR_SUCCESS) return res;
    size_t byte_length = (bit_length + 7) / 8;
    if (codec->position + byte_length > codec->size) return AXDR_ERROR_BUFFER_OVERFLOW;
    memcpy(codec->buffer + codec->position, bits, byte_length);
    codec->position += byte_length;
    return AXDR_SUCCESS;
}

// 可变长度位串解码
int axdr_decode_varbit_string(AXDR_CODEC* codec, uint8_t* bits, size_t* bit_length, size_t max_bits) {
    int32_t nbits = 0;
    int res = axdr_decode_varint(codec, &nbits);
    if (res != AXDR_SUCCESS) return res;
    if (nbits < 0 || (size_t)nbits > max_bits) return AXDR_ERROR_CONSTRAINT;
    size_t byte_length = ((size_t)nbits + 7) / 8;
    if (codec->position + byte_length > codec->size) return AXDR_ERROR_BUFFER_OVERFLOW;
    memcpy(bits, codec->buffer + codec->position, byte_length);
    codec->position += byte_length;
    *bit_length = (size_t)nbits;
    return AXDR_SUCCESS;
}
