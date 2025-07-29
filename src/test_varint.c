#include "axdr.h"
#include <stdio.h>
#include <string.h>

void test_varint() {
    printf("\nTesting VarInt Encoding/Decoding...\n");
    int32_t test_values[] = {0, 1, -1, 127, 128, 255, 16384, 2147483647, -2147483648};
    size_t n = sizeof(test_values)/sizeof(test_values[0]);
    uint8_t buffer[32];
    for (size_t i = 0; i < n; ++i) {
        memset(buffer, 0, sizeof(buffer));
        AXDR_CODEC* codec = axdr_codec_init(buffer, sizeof(buffer));
        int result = axdr_encode_varint(codec, test_values[i]);
        if (result != AXDR_SUCCESS) {
            printf("VarInt encode failed: %d\n", result);
            axdr_codec_cleanup(codec);
            continue;
        }
        int32_t decoded = 0;
        codec->position = 0;
        result = axdr_decode_varint(codec, &decoded);
        if (result != AXDR_SUCCESS) {
            printf("VarInt decode failed: %d\n", result);
        } else if (decoded != test_values[i]) {
            printf("VarInt test failed: value %d, decoded %d\n", test_values[i], decoded);
        } else {
            printf("VarInt test passed: value %d\n", test_values[i]);
        }
        axdr_codec_cleanup(codec);
    }
}

int main() {
    test_varint();
    return 0;
}
