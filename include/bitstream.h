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
 * @brief Creates and initializes a new BitWriter instance.
 *
 * The returned writer must be freed with bitwriter_destroy() to avoid
 * memory leaks.
 *
 * @return A pointer to the new BitWriter, or NULL if allocation fails.
 */
struct BitWriter *bitwriter_create();

/**
 * @brief Frees all memory associated with a BitWriter.
 *
 * Safely handles NULL pointers for the writer and its internal members.
 * After this call, the writer pointer is invalid and should not be used.
 *
 * @param w A pointer to the BitWriter to be destroyed.
 */
void bitwriter_destroy(struct BitWriter *w);

/**
 * @brief Flushes any remaining bits and returns the resulting byte array.
 *
 * This function finalizes the bitstream. It pushes the last partial byte
 * to the array if necessary. The caller does NOT take ownership of the
 * returned pointer; it is still owned by the BitWriter.
 *
 * @param w A pointer to the BitWriter.
 * @return A pointer to the ByteArray containing the written data, or NULL on
 * error.
 */
struct ByteArray *bitwriter_bytearray(struct BitWriter *w);

/**
 * @brief Writes a specified number of bytes / bits from a source byte array.
 *
 * Iterates through the source bytes and writes them to the bitstream.
 * It handles writing full bytes efficiently and then any remaining bits
 * from the final partial byte.
 *
 * @param w The BitWriter instance.
 * @param bytes The source ByteArray containing the data to write.
 * @param bits The total number of bits to write from the source array.
 * @return 1 on success, 0 on invalid arguments or memory allocation failure.
 */
int bitwriter_write(struct BitWriter *w, struct ByteArray *src, uint64_t bits);

/**
 * @brief Creates a BitReader from a ByteArray.
 *
 * @param bytes Pointer to the ByteArray to read from.
 * @return A new BitReader instance, or NULL on failure.
 */
struct BitReader *bitreader_create(struct ByteArray *bytes);

/**
 * @brief Frees a BitReader instance.
 *
 * @param r Pointer to the BitReader to destroy.
 */
void bitreader_destroy(struct BitReader *r);

/**
 * @brief Reads bits from the BitReader.
 *
 * Reads bits in MSB-first order, starting at the current position. Advances the
 * read position accordingly.
 *
 * @param r The BitReader instance.
 * @param bits Number of bits to read
 * @return ByteArray of the bits read, pad right on success, NULL on failure.
 */
struct ByteArray *bitreader_read(struct BitReader *r, uint64_t bits);

#endif
