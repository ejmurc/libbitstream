#include <stdlib.h>
#include <string.h>

#include "bitstream.h"
#include "unity.h"

void setUp(void) {}
void tearDown(void) {}

void test_roundtrip_single_byte(void) {
  struct BitWriter *writer = bitwriter_malloc();
  uint8_t data = 0xFF;
  struct ByteArray src = {&data, 1};
  bitwriter_write(writer, &src, 8);
  const struct ByteArray *written = bitwriter_bytearray(writer);
  struct BitReader *reader = bitreader_malloc((struct ByteArray *)written);
  struct ByteArray *result = bitreader_read(reader, 8);
  TEST_ASSERT_NOT_NULL(result);
  TEST_ASSERT_EQUAL(1, result->len);
  TEST_ASSERT_EQUAL_HEX8(0xFF, result->data[0]);
  bitwriter_free(writer);
  bitreader_free(reader);
  free(result->data);
  free(result);
}

void test_roundtrip_split_bytes(void) {
  struct BitWriter *writer = bitwriter_malloc();
  uint8_t data_a[] = {0x2A, 0xAA};  // 0x2AAA (0010101010101010)
  struct ByteArray arr_a = {data_a, 2};
  bitwriter_write(writer, &arr_a, 14);  // Buffer: (00101010 101010_00)
  uint8_t data_b[] = {0xA8};            // 0xA8 (10101000)
  struct ByteArray arr_b = {data_b, 1};
  bitwriter_write(writer, &arr_b, 6);  // Buffer: (00101010 10101010 1010_0000)

  const struct ByteArray *written = bitwriter_bytearray(writer);
  struct BitReader *reader = bitreader_malloc((struct ByteArray *)written);
  struct ByteArray *read_a = bitreader_read(reader, 14);
  TEST_ASSERT_NOT_NULL(read_a);
  TEST_ASSERT_EQUAL(2, read_a->len);
  TEST_ASSERT_EQUAL_HEX8(0x2A, read_a->data[0]);  // Expected: 00101010
  TEST_ASSERT_EQUAL_HEX8(0x2A, read_a->data[1]);  // Expected: 00_101010

  // Buffer: 00101010 101010*10 1010_0000

  struct ByteArray *read_b = bitreader_read(reader, 6);
  TEST_ASSERT_NOT_NULL(read_b);
  TEST_ASSERT_EQUAL(1, read_b->len);
  TEST_ASSERT_EQUAL_HEX8(0x2A,
                         read_b->data[0]);  // Expected: 00_101010 Was 000000_10

  bitwriter_free(writer);
  bitreader_free(reader);
  free(read_a->data);
  free(read_a);
  free(read_b->data);
  free(read_b);
}

int main(void) {
  UNITY_BEGIN();

  RUN_TEST(test_roundtrip_single_byte);
  RUN_TEST(test_roundtrip_split_bytes);

  return UNITY_END();
}
