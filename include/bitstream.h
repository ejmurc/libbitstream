#ifndef BITSTREAM_H
#define BITSTREAM_H

#include <stdint.h>
#include <string.h>

struct BitWriter {
  uint8_t *data;
  uint64_t capacity;
  uint64_t byte;
  uint8_t bit;
};

struct BitReader {
  const uint8_t *data;
  uint64_t length;
  uint64_t byte;
  uint8_t bit;
};

/**
 * @brief Initialize a bit writer with a buffer.
 * @param w Bit writer to initialize
 * @param buffer Output buffer
 * @param capacity Buffer capacity in bytes
 */
void bitwriter_init(struct BitWriter *w, uint8_t *buffer, uint64_t capacity);

/**
 * @brief Write bits from source buffer to bit writer.
 * @param w Bit writer
 * @param src Source buffer
 * @param src_capacity Source buffer capacity in bytes
 * @param bits Number of bits to write
 * @return 0 on success, non-zero on error
 */
int bitwriter_write(struct BitWriter *w, const uint8_t *src,
                    uint64_t src_capacity, uint64_t bits);

/**
 * @brief Get number of bytes written (including partial bytes).
 * @param w Bit writer
 * @return Total bytes containing data
 */
uint64_t bitwriter_length(struct BitWriter *w);

/**
 * @brief Initialize a bit reader with a buffer.
 * @param r Bit reader to initialize
 * @param buffer Input buffer
 * @param length Buffer length in bytes
 */
void bitreader_init(struct BitReader *r, const uint8_t *buffer,
                    uint64_t length);

/**
 * @brief Read bits from bit reader to destination buffer.
 * @param r Bit reader
 * @param dst Destination buffer
 * @param dst_capacity Destination buffer capacity in bytes
 * @param bits Number of bits to read
 * @return 0 on success, non-zero on error
 */
int bitreader_read(struct BitReader *r, uint8_t *dst, uint64_t dst_capacity,
                   uint64_t bits);

#endif
