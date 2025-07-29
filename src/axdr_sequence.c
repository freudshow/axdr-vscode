#include "axdr.h"
#include <stddef.h>
#include <string.h>

// SEQUENCE编解码函数
int axdr_encode_sequence(AXDR_CODEC* codec, const void* sequence,
                        AXDR_ENCODE_FIELD* encoders, size_t fieldCount) {
    if (!codec || !sequence || !encoders) {
        return AXDR_ERROR_INVALID_VALUE;
    }

    // sequence是一个指向字段数组的指针
    const void* const* fields = (const void* const*)sequence;

    for (size_t i = 0; i < fieldCount; i++) {
        if (!encoders[i] || !fields[i]) {
            return AXDR_ERROR_INVALID_VALUE;
        }

        int result = encoders[i](codec, fields[i]);
        if (result != AXDR_SUCCESS) {
            return result;
        }
    }

    return AXDR_SUCCESS;
}

int axdr_decode_sequence(AXDR_CODEC* codec, void* sequence,
                        AXDR_DECODE_FIELD* decoders, size_t fieldCount) {
    if (!codec || !sequence || !decoders) {
        return AXDR_ERROR_INVALID_VALUE;
    }

    // sequence是一个指向字段数组的指针
    void** fields = (void**)sequence;

    for (size_t i = 0; i < fieldCount; i++) {
        if (!decoders[i] || !fields[i]) {
            return AXDR_ERROR_INVALID_VALUE;
        }

        int result = decoders[i](codec, fields[i]);
        if (result != AXDR_SUCCESS) {
            return result;
        }
    }

    return AXDR_SUCCESS;
}

// SEQUENCE OF编解码函数
int axdr_encode_sequence_of(AXDR_CODEC* codec, const AXDR_SEQUENCE_OF* sequence,
                           AXDR_ENCODE_FIELD elementEncoder) {
    if (!codec || !sequence || !elementEncoder || !sequence->elements) {
        return AXDR_ERROR_INVALID_VALUE;
    }

    // 编码元素个数
    int result = axdr_encode_unsigned(codec, sequence->count, sequence->maxCount);
    if (result != AXDR_SUCCESS) {
        return result;
    }

    // 编码每个元素
    const char* elementPtr = (const char*)sequence->elements;
    for (size_t i = 0; i < sequence->count; i++) {
        result = elementEncoder(codec, elementPtr + i * sequence->elementSize);
        if (result != AXDR_SUCCESS) {
            return result;
        }
    }

    return AXDR_SUCCESS;
}

int axdr_decode_sequence_of(AXDR_CODEC* codec, AXDR_SEQUENCE_OF* sequence,
                           AXDR_DECODE_FIELD elementDecoder) {
    if (!codec || !sequence || !elementDecoder || !sequence->elements) {
        return AXDR_ERROR_INVALID_VALUE;
    }

    // 解码元素个数
    uint32_t count;
    int result = axdr_decode_unsigned(codec, &count, sequence->maxCount);
    if (result != AXDR_SUCCESS) {
        return result;
    }

    if (count > sequence->maxCount) {
        return AXDR_ERROR_CONSTRAINT;
    }

    sequence->count = count;

    // 解码每个元素
    char* elementPtr = (char*)sequence->elements;
    for (size_t i = 0; i < count; i++) {
        result = elementDecoder(codec, elementPtr + i * sequence->elementSize);
        if (result != AXDR_SUCCESS) {
            return result;
        }
    }

    return AXDR_SUCCESS;
}

// 带参数的SEQUENCE编解码函数
int axdr_encode_sequence_with_params(AXDR_CODEC* codec, 
                                   const AXDR_ENCODE_PARAMS* params,
                                   size_t paramCount,
                                   AXDR_FIELD_ENCODER encoder) {
    if (!codec || !params || !encoder) {
        return AXDR_ERROR_INVALID_VALUE;
    }
    
    for (size_t i = 0; i < paramCount; i++) {
        if (!params[i].value) {
            return AXDR_ERROR_INVALID_VALUE;
        }
        
        int result = encoder(codec, params[i].value, params[i].type);
        if (result != AXDR_SUCCESS) {
            return result;
        }
    }
    
    return AXDR_SUCCESS;
}

int axdr_decode_sequence_with_params(AXDR_CODEC* codec,
                                   AXDR_ENCODE_PARAMS* params,
                                   size_t paramCount,
                                   AXDR_FIELD_ENCODER encoder) {
    if (!codec || !params || !encoder) {
        return AXDR_ERROR_INVALID_VALUE;
    }
    
    for (size_t i = 0; i < paramCount; i++) {
        if (!params[i].value) {
            return AXDR_ERROR_INVALID_VALUE;
        }
        
        int result = encoder(codec, params[i].value, params[i].type);
        if (result != AXDR_SUCCESS) {
            return result;
        }
    }
    
    return AXDR_SUCCESS;
}
