#ifndef BITSTREAM_H
#define BITSTREAM_H

#include <stdint.h>
#include <stdlib.h>
#include <assert.h>

struct ByteArray {
  uint8_t *data;
  size_t len;
};

struct BitWriter {
  struct ByteArray *bytes;
  uint8_t current;
  uint8_t pos;
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
 * @return A pointer to the ByteArray containing the written data, or NULL on error.
 */
struct ByteArray *bitwriter_bytearray(struct BitWriter *w);

/**
 * @brief Writes a specified number of bits from a source byte array.
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
int bitwriter_write(struct BitWriter *w, struct ByteArray *bytes, uint32_t bits);

#endif
