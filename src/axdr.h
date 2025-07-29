#ifndef AXDR_H
#define AXDR_H

#include <stdint.h>
#include <stdbool.h>
#include <time.h>

// 基本数据类型定义
typedef uint8_t  AXDR_UINT8;
typedef uint16_t AXDR_UINT16;
typedef uint32_t AXDR_UINT32;
typedef int8_t   AXDR_INT8;
typedef int16_t  AXDR_INT16;
typedef int32_t  AXDR_INT32;
typedef bool     AXDR_BOOL;
typedef uint8_t  AXDR_OCTET_STRING;
typedef uint8_t  AXDR_BIT_STRING;
typedef char     AXDR_VISIBLE_STRING;
typedef time_t   AXDR_GENERALIZED_TIME;

// SEQUENCE OF类型定义
typedef struct {
    void*  elements;     // 元素数组
    size_t elementSize;  // 每个元素的大小
    size_t count;        // 元素个数
    size_t maxCount;     // 最大元素个数
} AXDR_SEQUENCE_OF;

// 编码上下文结构
typedef struct {
    uint8_t* buffer;     // 编码缓冲区
    size_t   size;       // 缓冲区大小
    size_t   position;   // 当前位置
    int      error;      // 错误码
} AXDR_CODEC;

// 编码参数结构
typedef struct {
    const void* value;   // 字段值指针
    int type;           // 字段类型
} AXDR_ENCODE_PARAMS;

// 错误码定义
#define AXDR_SUCCESS                 0
#define AXDR_ERROR_BUFFER_OVERFLOW  -1
#define AXDR_ERROR_INVALID_LENGTH   -2
#define AXDR_ERROR_INVALID_VALUE    -3
#define AXDR_ERROR_CONSTRAINT       -4
#define AXDR_ERROR_INVALID_TYPE     -5

// 编码函数声明
int axdr_encode_integer(AXDR_CODEC* codec, int32_t value, int32_t min, int32_t max);
int axdr_encode_unsigned(AXDR_CODEC* codec, uint32_t value, uint32_t max);
int axdr_encode_boolean(AXDR_CODEC* codec, bool value);
int axdr_encode_enum(AXDR_CODEC* codec, int value, int count);
int axdr_encode_bit_string(AXDR_CODEC* codec, const uint8_t* bits, size_t length);
int axdr_encode_octet_string(AXDR_CODEC* codec, const uint8_t* octets, size_t length);
int axdr_encode_visible_string(AXDR_CODEC* codec, const char* str, size_t max_length);
int axdr_encode_generalized_time(AXDR_CODEC* codec, time_t time);
int axdr_encode_null(AXDR_CODEC* codec);

// 解码函数声明
int axdr_decode_integer(AXDR_CODEC* codec, int32_t* value, int32_t min, int32_t max);
int axdr_decode_unsigned(AXDR_CODEC* codec, uint32_t* value, uint32_t max);
int axdr_decode_boolean(AXDR_CODEC* codec, bool* value);
int axdr_decode_enum(AXDR_CODEC* codec, int* value, int count);
int axdr_decode_bit_string(AXDR_CODEC* codec, uint8_t* bits, size_t* length);
int axdr_decode_octet_string(AXDR_CODEC* codec, uint8_t* octets, size_t* length);
int axdr_decode_visible_string(AXDR_CODEC* codec, char* str, size_t max_length);
int axdr_decode_generalized_time(AXDR_CODEC* codec, time_t* time);
int axdr_decode_null(AXDR_CODEC* codec);

// SEQUENCE编解码函数类型定义
typedef int (*AXDR_ENCODE_FIELD)(AXDR_CODEC* codec, const void* field);
typedef int (*AXDR_DECODE_FIELD)(AXDR_CODEC* codec, void* field);
typedef int (*AXDR_FIELD_ENCODER)(AXDR_CODEC* codec, const void* field, int field_type);

// SEQUENCE编解码函数
int axdr_encode_sequence(AXDR_CODEC* codec, const void* sequence, 
                        AXDR_ENCODE_FIELD* encoders, size_t fieldCount);
int axdr_decode_sequence(AXDR_CODEC* codec, void* sequence,
                        AXDR_DECODE_FIELD* decoders, size_t fieldCount);

// 新的基于参数的SEQUENCE编解码函数
int axdr_encode_sequence_with_params(AXDR_CODEC* codec, 
                                   const AXDR_ENCODE_PARAMS* params,
                                   size_t paramCount,
                                   AXDR_FIELD_ENCODER encoder);
int axdr_decode_sequence_with_params(AXDR_CODEC* codec,
                                   AXDR_ENCODE_PARAMS* params,
                                   size_t paramCount,
                                   AXDR_FIELD_ENCODER encoder);

// SEQUENCE OF编解码函数
int axdr_encode_sequence_of(AXDR_CODEC* codec, const AXDR_SEQUENCE_OF* sequence,
                           AXDR_ENCODE_FIELD elementEncoder);
int axdr_decode_sequence_of(AXDR_CODEC* codec, AXDR_SEQUENCE_OF* sequence,
                           AXDR_DECODE_FIELD elementDecoder);

// 上下文操作函数
AXDR_CODEC* axdr_codec_init(uint8_t* buffer, size_t size);
void axdr_codec_cleanup(AXDR_CODEC* codec);

#endif // AXDR_H
