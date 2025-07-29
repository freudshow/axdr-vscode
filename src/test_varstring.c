#include "axdr.h"
#include <stdio.h>
#include <string.h>

void test_varstrings() {
    printf("\nTesting VarOctet/VarVisible/VarBit Encoding/Decoding...\n");
    uint8_t octet_in[8] = {1,2,3,4,5,6,7,8};
    uint8_t octet_out[8] = {0};
    size_t len = 8, outlen = 0;
    uint8_t buffer[32];
    AXDR_CODEC* codec = axdr_codec_init(buffer, sizeof(buffer));
    int res = axdr_encode_varoctet_string(codec, octet_in, len);
    codec->position = 0;
    res |= axdr_decode_varoctet_string(codec, octet_out, &outlen, 8);
    printf("VarOctet: %s\n", (res==0 && outlen==len && memcmp(octet_in, octet_out, len)==0)?"pass":"fail");
    axdr_codec_cleanup(codec);

    char* str = "hello可变串";
    char str_out[32] = {0};
    size_t slen = strlen(str), slen_out = 0;
    memset(buffer, 0, sizeof(buffer));
    codec = axdr_codec_init(buffer, sizeof(buffer));
    res = axdr_encode_varvisible_string(codec, str);
    codec->position = 0;
    res |= axdr_decode_varvisible_string(codec, str_out, &slen_out, 31);
    printf("VarVisible: %s\n", (res==0 && slen==slen_out && strcmp(str,str_out)==0)?"pass":"fail");
    axdr_codec_cleanup(codec);

    uint8_t bits_in[2] = {0xAA, 0x0F};
    uint8_t bits_out[2] = {0};
    size_t bitlen = 12, bitlen_out = 0;
    memset(buffer, 0, sizeof(buffer));
    codec = axdr_codec_init(buffer, sizeof(buffer));
    res = axdr_encode_varbit_string(codec, bits_in, bitlen);
    codec->position = 0;
    res |= axdr_decode_varbit_string(codec, bits_out, &bitlen_out, 16);
    printf("VarBit: %s\n", (res==0 && bitlen==bitlen_out && memcmp(bits_in, bits_out, 2)==0)?"pass":"fail");
    axdr_codec_cleanup(codec);
}

int main() {
    test_varstrings();
    return 0;
}
