#include "bitstream.h"

#include <stdio.h>

static uint32_t min(uint32_t a, uint32_t b) { return a < b ? a : b; }

void bitwriter_init(struct BitWriter *w, uint8_t *buffer, uint64_t capacity) {
  if (!w || !buffer || capacity == 0) return;
  w->data = buffer;
  w->capacity = capacity;
  w->byte = 0;
  w->bit = 0;
}

static int write_u8(struct BitWriter *w, uint8_t value, uint8_t bits) {
  if (!w || bits == 0 || bits > 8 || w->byte >= w->capacity) {
    return 1;
  }

  if (w->bit == 0) {
    w->data[w->byte] = 0;
  }

  value &= (1 << bits) - 1;

  if (w->bit + bits <= 8) {
    w->data[w->byte] |= value << (8 - w->bit - bits);
    w->bit += bits;
    if (w->bit == 8) {
      w->byte++;
      w->bit = 0;
    }
  } else {
    uint8_t nbits = 8 - w->bit;
    uint8_t rbits = bits - nbits;
    w->data[w->byte] |= value >> rbits;
    w->byte++;
    if (w->byte >= w->capacity) return 1;
    uint8_t mask = (1 << rbits) - 1;
    w->data[w->byte] = (value & mask) << (8 - rbits);
    w->bit = rbits;
  }
  return 0;
}

int bitwriter_write(struct BitWriter *w, const uint8_t *src,
                    uint64_t src_capacity, uint64_t bits) {
  if (!w || !w->data || !src) {
    return 1;
  }
  if (bits == 0) {
    return 0;
  }
  const uint64_t tbits = min(src_capacity * 8, bits);
  const uint64_t fbytes = tbits / 8;
  uint64_t i;
  for (i = 0; i < fbytes; i++) {
    if (write_u8(w, src[i], 8)) {
      return 1;
    }
  }
  const uint64_t rbits = tbits % 8;
  if (rbits) {
    uint8_t shifted = src[fbytes] & ((1 << rbits) - 1);
    if (write_u8(w, shifted, rbits)) {
      return 1;
    }
  }
  return 0;
}

uint64_t bitwriter_length(struct BitWriter *w) {
  if (!w) return 1;
  return w->byte + (w->bit > 0);
}

void bitreader_init(struct BitReader *r, const uint8_t *buffer,
                    uint64_t length) {
  if (!r || !buffer || length == 0) return;
  r->data = buffer;
  r->length = length;
  r->byte = 0;
  r->bit = 0;
}

int bitreader_read(struct BitReader *r, uint8_t *dst, uint64_t dst_capacity,
                   uint64_t bits) {
  if (!r || !dst || dst_capacity < (bits + 7) / 8) {
    return 1;
  }
  if (bits == 0) {
    return 0;
  }
  uint64_t bytec = (r->bit + bits + 7) / 8;
  if (r->byte + bytec > r->length) {
    return 1;
  }
  memset(dst, 0, (bits + 7) / 8);
  uint64_t processed = 0;
  while (processed < bits) {
    uint8_t bitc = min(bits - processed, 8 - r->bit);
    uint8_t mask = ((1 << bitc) - 1) << (8 - r->bit - bitc);
    uint8_t data = (r->data[r->byte] & mask) >> (8 - r->bit - bitc);
    uint64_t out_offset = bits - processed - bitc;
    dst[out_offset / 8] |= data << (out_offset % 8);
    processed += bitc;
    r->bit += bitc;
    if (r->bit >= 8) {
      r->bit = 0;
      r->byte++;
    }
  }
  return 0;
}
