#include <stdio.h>

#include "bitstream.h"
#include "unity.h"

void setUp(void) {}
void tearDown(void) {}

// Aligned byte tests
void test_uint8_cast_aligned_roundtrip(void) {
  uint8_t buffer[1];
  struct BitWriter writer;
  struct BitReader reader;

  uint8_t value = 0x12;
  bitwriter_init(&writer, buffer, sizeof(buffer));
  TEST_ASSERT_EQUAL_INT(0, bitwriter_write(&writer, &value, 1, 8));

  bitreader_init(&reader, buffer, bitwriter_length(&writer));

  uint8_t read_buffer;
  TEST_ASSERT_EQUAL_INT(0, bitreader_read(&reader, &read_buffer, 1, 8));
  TEST_ASSERT_EQUAL_UINT8(value, read_buffer);
}

void test_uint64_cast_aligned_roundtrip(void) {
  uint8_t buffer[8];
  struct BitWriter writer;
  struct BitReader reader;

  uint64_t value = 0x12345678;
  bitwriter_init(&writer, buffer, sizeof(buffer));
  TEST_ASSERT_EQUAL_INT(
      0, bitwriter_write(&writer, (uint8_t *)&value, sizeof(uint64_t), 64));

  bitreader_init(&reader, buffer, bitwriter_length(&writer));

  uint64_t read_buffer;
  TEST_ASSERT_EQUAL_INT(0,
                        bitreader_read(&reader, (uint8_t *)&read_buffer, 1, 8));
  TEST_ASSERT_EQUAL_UINT8(value, read_buffer);
}

// Unaligned byte tests
void test_uint8_cast_unaligned_roundtrip(void) {
    uint8_t buffer[2] = {0};
    struct BitWriter writer;
    struct BitReader reader;
    bitwriter_init(&writer, buffer, sizeof(buffer));

    uint8_t alignment_byte = 0b00000111; // Bottom 3 bits: 111
    TEST_ASSERT_EQUAL_INT(0, bitwriter_write(&writer, &alignment_byte, 1, 3));

    uint8_t value = 0b00010010;
    TEST_ASSERT_EQUAL_INT(0, bitwriter_write(&writer, &value, 1, 8));

    bitreader_init(&reader, buffer, bitwriter_length(&writer));

    uint8_t alignment_read;
    TEST_ASSERT_EQUAL_INT(0, bitreader_read(&reader, &alignment_read, 1, 3));

    TEST_ASSERT_EQUAL_UINT8(alignment_byte, alignment_read); // Should be equivalent since only bottom 3 bits written
    
    uint8_t read_buffer;
    TEST_ASSERT_EQUAL_INT(0, bitreader_read(&reader, &read_buffer, 1, 8));
    TEST_ASSERT_EQUAL_UINT8(value, read_buffer); // Fails, Expected 18 Was 66
}

int main() {
  UNITY_BEGIN();
  RUN_TEST(test_uint8_cast_aligned_roundtrip);
  RUN_TEST(test_uint64_cast_aligned_roundtrip);
  RUN_TEST(test_uint8_cast_unaligned_roundtrip);
  return UNITY_END();
}
