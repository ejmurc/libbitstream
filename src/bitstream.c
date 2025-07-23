#include "bitstream.h"

/**
 * @brief Finds the minimum of two 32-bit unsigned integers.
 * @param a The first integer.
 * @param b The second integer.
 * @return The smaller of the two integers.
 */
static uint32_t min(uint32_t a, uint32_t b) {
  return a < b ? a : b;
}

/**
 * @brief Appends a single byte to a dynamic byte array.
 *
 * This function handles reallocating the underlying data buffer.
 *
 * @param arr A pointer to the ByteArray struct.
 * @param byte The byte to append.
 * @return 1 on success, 0 if memory allocation fails.
 */
static int push_byte(struct ByteArray *arr, uint8_t byte) {
  arr->len ++;
  uint8_t *ndata = realloc(arr->data, arr->len);
  if (!ndata) {
    arr->len--;
    return 0;
  }
  ndata[arr->len - 1] = byte;
  arr->data = ndata;
  return 1;
}

struct BitWriter *bitwriter_create() {
  struct BitWriter *w = calloc(1, sizeof(struct BitWriter));
  if (!w) {
    return NULL;
  }

  w->bytes = calloc(1, sizeof(struct ByteArray));
  if (!w->bytes) {
    free(w);
    return NULL;
  }
  return w;
}

void bitwriter_destroy(struct BitWriter *w) {
  if (!w) {
    return;
  }
  if (w->bytes) {
    free(w->bytes->data);
    free(w->bytes);
  }
  free(w);
}

struct ByteArray *bitwriter_bytes(struct BitWriter *w) {
  if (!w) {
    return NULL;
  }
  if (w->pos > 0) {
    if (!push_byte(w->bytes, w->current)) {
      return NULL;
    }
  }
  return w->bytes;
}

/**
 * @brief Writes up to 8 bits to the stream.
 *
 * This is the core internal function that handles packing bits into bytes
 * and manages byte boundaries.
 *
 * @param w The BitWriter instance.
 * @param value The byte value containing the bits to write (in the LSB).
 * @param bits The number of bits to write from the value (must be > 0 and <= 8).
 * @return 1 on success, 0 on invalid arguments or memory allocation failure.
 */
static int write_u8(struct BitWriter *w, uint8_t value, uint8_t bits) {
  if (!w || bits == 0 || bits > 8) {
    return 0;
  }

  if (w->pos + bits > 8) {
    uint8_t rbits = bits - (8 - w->pos);
    w->current |= value >> rbits;
    if (!push_byte(w->bytes, w->current)) return 0;
    w->current = (value & ((1 << rbits) - 1)) << (8 - rbits);
    w->pos = rbits;
  } else {
    uint8_t shift = 8 - w->pos - bits;
    w->current |= (value & ((1 << bits) - 1)) << shift;
    w->pos += bits;
    if (w->pos == 8) {
      if (!push_byte(w->bytes, w->current)) return 0;
      w->current = 0;
      w->pos = 0;
    }
  }
  return 1;
}

int bitwriter_write(struct BitWriter *w, struct ByteArray *bytes, uint32_t bits) {
    if (!w || !w->bytes || !bytes) {
    return 0;
  }

  const size_t total_bits = min(bytes->len * 8, bits);
  const size_t full_bytes = total_bits / 8;

  size_t i;
  for (i = 0; i < full_bytes; i++) {
    if (!write_u8(w, bytes->data[i], 8)) {
      return 0;
    }
  }

  const size_t remaining_bits = total_bits % 8;
  if (remaining_bits) {
    uint8_t shifted = bytes->data[full_bytes] >> (8 - remaining_bits);
    if (!write_u8(w, shifted, remaining_bits)) {
      return 0;
    }
  }
  return 1;
}
