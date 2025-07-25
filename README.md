# libbitstream

**Zero-allocation bitstream library in C**

`libbitstream` provides a simple, fast, and zero-allocation API for reading and writing arbitrary-length bit sequences from byte-aligned buffers. Ideal for embedded systems, custom binary protocols, and performance-critical applications.

## Features

- Zero heap allocations — you manage the memory
- Bit-precise reading and writing
- MSB-first bit ordering
- Small and portable (single header + source file)
- C89 compatible

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
    bitwriter_write(&writer, &data, 1, 5); // Write 5 bits

    struct BitReader reader;
    bitreader_init(&reader, buffer, bitwriter_length(&writer));

    uint8_t out = 0;
    bitreader_read(&reader, &out, 1, 5);

    printf("Read bits: 0x%02X\n", out); // Output: 0x15
}
````

## API Reference

### Writer

Initializes a `BitWriter` with a user-provided buffer.
```c
void bitwriter_init(struct BitWriter *w, uint8_t *buffer, uint64_t capacity);
```

Writes `bits` number of bits from `src` into the writer buffer.
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
Reads `bits` number of bits into `dst`.
```c
int bitreader_read(struct BitReader *r, uint8_t *dst, uint64_t dst_capacity, uint64_t bits);
```

## Design Notes

* All operations are MSB-first (bit 7 is written/read first).
* No dynamic allocation — memory must be managed externally.
* Bit offsets persist across writes/reads (no automatic alignment).
