#include <stdlib.h>
#include <string.h>

#include "bitstream.h"
#include "unity.h"

void setUp(void) {}
void tearDown(void) {}

void test_roundtrip_single_byte(void) {
  uint8_t write_buffer[32];
  uint8_t read_buffer[32];

  struct BitWriter writer;
  bitwriter_init(&writer, write_buffer, sizeof(write_buffer));

  uint8_t data = 0xFF;
  memset(write_buffer, 0, sizeof(write_buffer));
  int write_result = bitwriter_write(&writer, &data, 1, 8);
  TEST_ASSERT_EQUAL(1, write_result);

  struct BitReader reader;
  bitreader_init(&reader, write_buffer, bitwriter_length(&writer));

  memset(read_buffer, 0, sizeof(read_buffer));
  int read_result =
      bitreader_read(&reader, read_buffer, sizeof(read_buffer), 8);
  TEST_ASSERT_EQUAL(1, read_result);
  TEST_ASSERT_EQUAL_HEX8(0xFF, read_buffer[0]);
}

void test_roundtrip_split_bytes(void) {
  uint8_t write_buffer[32];
  uint8_t read_buffer_a[32];
  uint8_t read_buffer_b[32];

  struct BitWriter writer;
  bitwriter_init(&writer, write_buffer, sizeof(write_buffer));

  uint8_t data_a[] = {0x2A, 0xAA};  // 0x2AAA (0010101010101010)
  memset(write_buffer, 0, sizeof(write_buffer));
  int write_result_a =
      bitwriter_write(&writer, data_a, 2, 14);  // Buffer: (00101010 101010_00)
  TEST_ASSERT_EQUAL(1, write_result_a);

  uint8_t data_b[] = {0xA8};  // 0xA8 (10101000)
  int write_result_b = bitwriter_write(
      &writer, data_b, 1, 6);  // Buffer: (00101010 10101010 1010_0000)
  TEST_ASSERT_EQUAL(1, write_result_b);

  struct BitReader reader;
  bitreader_init(&reader, write_buffer, bitwriter_length(&writer));

  memset(read_buffer_a, 0, sizeof(read_buffer_a));
  int read_result_a =
      bitreader_read(&reader, read_buffer_a, sizeof(read_buffer_a), 14);
  TEST_ASSERT_EQUAL(1, read_result_a);
  TEST_ASSERT_EQUAL_HEX8(0x2A, read_buffer_a[0]);  // Expected: 00101010
  TEST_ASSERT_EQUAL_HEX8(0x2A, read_buffer_a[1]);  // Expected: 00_101010

  // Buffer: 00101010 101010*10 1010_0000
  memset(read_buffer_b, 0, sizeof(read_buffer_b));
  int read_result_b =
      bitreader_read(&reader, read_buffer_b, sizeof(read_buffer_b), 6);
  TEST_ASSERT_EQUAL(1, read_result_b);
  TEST_ASSERT_EQUAL_HEX8(0x2A, read_buffer_b[0]);  // Expected: 00_101010
}

int main(void) {
  UNITY_BEGIN();
  RUN_TEST(test_roundtrip_single_byte);
  RUN_TEST(test_roundtrip_split_bytes);
  return UNITY_END();
}
