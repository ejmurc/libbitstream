#include "bitstream.h"

/**
 * @brief Finds the minimum of two 32-bit unsigned integers.
 * @param a The first integer.
 * @param b The second integer.
 * @return The smaller of the two integers.
 */
static uint32_t min(uint32_t a, uint32_t b) { return a < b ? a : b; }

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
  arr->len++;
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
  if (w->bit > 0) {
    if (!push_byte(w->bytes, w->current)) {
      return NULL;
    }
  }
  return w->bytes;
}

/**
 * @brief Writes up to 8 bits to a BitWriter.
 *
 * @param w The BitWriter.
 * @param value The byte value containing the bits to write (in the LSB).
 * @param bits The number of bits to write from the value (must be > 0 and <=
 * 8).
 * @return 1 on success, 0 on invalid arguments or memory allocation failure.
 */
static int write_u8(struct BitWriter *w, uint8_t value, uint8_t bits) {
  if (!w || bits == 0 || bits > 8) {
    return 0;
  }
  if (w->bit + bits > 8) {
    uint8_t rbits = bits - (8 - w->bit);
    w->current |= value >> rbits;
    if (!push_byte(w->bytes, w->current)) return 0;
    w->current = (value & ((1 << rbits) - 1)) << (8 - rbits);
    w->bit = rbits;
  } else {
    uint8_t shift = 8 - w->bit - bits;
    w->current |= (value & ((1 << bits) - 1)) << shift;
    w->bit += bits;
    if (w->bit == 8) {
      if (!push_byte(w->bytes, w->current)) return 0;
      w->current = 0;
      w->bit = 0;
    }
  }
  return 1;
}

int bitwriter_write(struct BitWriter *w, struct ByteArray *src, uint64_t bits) {
  if (!w || !w->bytes || !src || bits == 0) {
    return 0;
  }
  const uint64_t tbits = min(src->len * 8, bits);
  const uint64_t fbytes = tbits / 8;
  uint64_t i;
  for (i = 0; i < fbytes; i++) {
    if (!write_u8(w, src->data[i], 8)) {
      return 0;
    }
  }
  const uint64_t rbits = tbits % 8;
  if (rbits) {
    uint8_t shifted = src->data[fbytes] >> (8 - rbits);
    if (!write_u8(w, shifted, rbits)) {
      return 0;
    }
  }
  return 1;
}

struct BitReader *bitreader_create(struct ByteArray *bytes) {
  struct BitReader *r = calloc(1, sizeof(struct BitReader));
  if (!r) {
    return NULL;
  }
  r->bytes = bytes;
  return r;
}

void bitreader_destroy(struct BitReader *r) {
  if (!r) {
    return;
  }
  if (r->bytes) {
    free(r->bytes->data);
    free(r->bytes);
  }
  free(r);
}

struct ByteArray *bitreader_read(struct BitReader *r, uint64_t bits) {
  if (!r || bits == 0) {
    return NULL;
  }
  uint64_t bytec = (r->bit + bits + 7) / 8;
  if (r->byte + bytec > r->bytes->len) {
    return NULL;
  }
  struct ByteArray *result = malloc(sizeof(struct ByteArray));
  if (!result) {
    return NULL;
  }
  result->len = (bits + 7) / 8;
  result->data = calloc(result->len, 1);
  if (!result->data) {
    free(result);
    return NULL;
  }
  uint64_t processed = 0;
  while (processed < bits) {
    uint8_t bitc = min(bits - processed, 8 - r->bit);
    uint8_t mask = ((1 << bitc) - 1) << r->bit;
    uint8_t data = (r->bytes->data[r->byte] & mask) >> r->bit;
    result->data[processed / 8] |= data << (processed % 8);
    processed += bitc;
    r->bit += bitc;
    if (r->bit >= 8) {
      r->bit = 0;
      r->byte++;
    }
  }
  return result;
}
