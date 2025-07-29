# A-XDR Encoding/Decoding Library

This is an implementation of the A-XDR (Abstract External Data Representation) encoding rules based on DLT 790.6-2010 standard.

## Features

- Integer type encoding/decoding with constraint checking
- Boolean type encoding/decoding
- Enumerated type encoding/decoding
- BitString type encoding/decoding
- OctetString type encoding/decoding
- Choice type support
- Tagged type support
- Optional type support
- Sequence type support
- SEQUENCE OF type support
- VisibleString type encoding/decoding
- GeneralizedTime type encoding/decoding
- ASN.1 NULL type support

## Building

To build the library and run tests:

```bash
mkdir build
cd build
cmake ..
make
```

To run the tests:

```bash
./test_axdr
```

## Usage Example

```c
#include "axdr.h"

// Initialize codec
uint8_t buffer[1024];
AXDR_CODEC* codec = axdr_codec_init(buffer, sizeof(buffer));

// Encode data
int32_t value = 42;
axdr_encode_integer(codec, value, INT32_MIN, INT32_MAX);

// Decode data
int32_t decoded;
axdr_decode_integer(codec, &decoded, INT32_MIN, INT32_MAX);

// Clean up
axdr_codec_cleanup(codec);
```

## Error Handling

The library uses the following error codes:

- `AXDR_SUCCESS`: Operation successful
- `AXDR_ERROR_BUFFER_OVERFLOW`: Buffer overflow occurred
- `AXDR_ERROR_INVALID_LENGTH`: Invalid length value
- `AXDR_ERROR_INVALID_VALUE`: Invalid value encountered
- `AXDR_ERROR_CONSTRAINT`: Constraint violation
- `AXDR_ERROR_INVALID_TYPE`: Invalid type encountered
