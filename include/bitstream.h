#ifndef BITSTREAM_H
#define BITSTREAM_H

#include <assert.h>
#include <stdint.h>
#include <stdlib.h>

struct ByteArray {
  uint8_t *data;
  uint64_t len;
};

struct BitWriter {
  struct ByteArray *bytes;
  uint8_t current;
  uint8_t bit;
};

struct BitReader {
  struct ByteArray *bytes;
  uint64_t byte;
  uint8_t bit;
};

/**
 * @brief Allocate and initialize a new BitWriter
 * @return Pointer to the newly allocated BitWriter, or NULL on failure
 */
struct BitWriter *bitwriter_malloc();

/**
 * @brief Free a BitWriter and its resources
 * @param w Pointer to the BitWriter to free
 */
void bitwriter_free(struct BitWriter *w);

/**
 * @brief Get the underlying ByteArray from a BitWriter
 * @param w Pointer to the BitWriter
 * @return Pointer to the ByteArray containing the written data
 */
const struct ByteArray *bitwriter_bytearray(struct BitWriter *w);

/**
 * @brief Write bits from a source ByteArray to a BitWriter
 * @param w Pointer to the destination BitWriter
 * @param src Pointer to the source ByteArray
 * @param bits Number of bits to write
 * @return 1 on success, 0 on failure
 */
int bitwriter_write(struct BitWriter *w, struct ByteArray *src, uint64_t bits);

/**
 * @brief Allocate and initialize a new BitReader
 * @param bytes Pointer to the ByteArray to read from
 * @return Pointer to the newly allocated BitReader, or NULL on failure
 */
struct BitReader *bitreader_malloc(struct ByteArray *bytes);

/**
 * @brief Free a BitReader
 * @param r Pointer to the BitReader to free
 * @note This does not free the underlying ByteArray
 */
void bitreader_free(struct BitReader *r);

/**
 * @brief Read bits from a BitReader into a new ByteArray
 * @param r Pointer to the BitReader
 * @param bits Number of bits to read
 * @return New ByteArray containing the read bits, or NULL on failure
 */
struct ByteArray *bitreader_read(struct BitReader *r, uint64_t bits);

#endif
