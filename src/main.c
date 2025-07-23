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

int main(void) {
  UNITY_BEGIN();

  RUN_TEST(test_roundtrip_single_byte);

  return UNITY_END();
}
