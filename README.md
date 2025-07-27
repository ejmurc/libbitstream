# libbitstream
**Zero-allocation bitstream library in C**

`libbitstream` is a zero-allocation bitstream library in C that reads and writes arbitrary-length fields to byte-aligned buffers. Ideal for custom binary protocols, serialization formats, and embedded systems.

## Features
- Zero heap allocations — you manage the memory
- Bit-precise reading and writing
- MSB-first bit ordering and packing
- Small and portable (single header + source file)
- C89 compatible (testing framework uses C99, though)

## Getting Started

### Installation
Just copy `include/bitstream.h` and `src/bitstream.c` into your project. No external dependencies.

### Example
```c
#include "bitstream.h"
#include <stdio.h>
#include <string.h>

int main() {
    uint8_t buffer[8] = {0};
    struct BitWriter writer;
    bitwriter_init(&writer, buffer, sizeof(buffer));
    
    uint8_t data = 0b10101010;
    bitwriter_write(&writer, &data, 1, 5); // Write bottom 5 bits: 01010
    
    struct BitReader reader;
    bitreader_init(&reader, buffer, bitwriter_length(&writer));
    
    uint8_t out = 0;
    bitreader_read(&reader, &out, 1, 5);
    printf("Read bits: 0x%02X\n", out); // Output: 0x0A (bottom 5 bits: 01010)
}
```

## API Reference

### Writer
Initializes a `BitWriter` with a user-provided buffer.
```c
void bitwriter_init(struct BitWriter *w, uint8_t *buffer, uint64_t capacity);
```

Writes `bits` number of bits from `src` into the writer buffer. Takes the least significant `bits` from the source data.
```c
int bitwriter_write(struct BitWriter *w, const uint8_t *src, uint64_t src_capacity, uint64_t bits);
```

Returns the number of bytes written, including the current partially filled byte.
```c
uint64_t bitwriter_length(struct BitWriter *w);
```

### Reader
Initializes a `BitReader` from a byte buffer.
```c
void bitreader_init(struct BitReader *r, const uint8_t *buffer, uint64_t length);
```

Reads `bits` number of bits into `dst`. Places the bits in the least significant positions of the destination.
```c
int bitreader_read(struct BitReader *r, uint8_t *dst, uint64_t dst_capacity, uint64_t bits);
```

## Design Notes
* Bits are packed MSB-first within each byte
* When writing N bits from source data, the least significant N bits are used
* When reading N bits, they are placed in the least significant N bits of the destination
* No dynamic allocation - memory must be managed externally
* Bit offsets persist across writes/reads (no automatic alignment)
* Functions return 0 on success, non-zero on error
