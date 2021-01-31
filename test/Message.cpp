#include "gtest/gtest.h"
#include "../src/Message.hpp"

TEST(Message, SerializeByte) {
  uint8_t expected = 214;

  uint8_t buffer[1];
  void *buf = &buffer;
  int buf_len = 1;
  message::write_byte(&buf, &buf_len, expected);

  EXPECT_EQ(0, buf_len);
  EXPECT_EQ(buffer + 1, buf);
  EXPECT_EQ(expected, buffer[0]);
}

TEST(Message, SerializeUnsignedInteger_Zero) {
  int buffer_size = 64;
  int number_to_serialize = 0;

  uint8_t buffer[buffer_size];
  void *buf = &buffer;
  int buf_len = sizeof(buffer);
  message::write_unsigned_integer(&buf, &buf_len, number_to_serialize);

  EXPECT_EQ(buffer_size - 1, buf_len);
  EXPECT_EQ(buffer + 1, buf);
  EXPECT_EQ(number_to_serialize, buffer[0]);
}

TEST(Message, SerializeUnsignedInteger_FitsTo1Byte) {
  int buffer_size = 64;
  int number_to_serialize = 127;

  uint8_t buffer[buffer_size];
  void *buf = &buffer;
  int buf_len = sizeof(buffer);
  message::write_unsigned_integer(&buf, &buf_len, number_to_serialize);

  EXPECT_EQ(buffer_size - 1, buf_len);
  EXPECT_EQ(buffer + 1, buf);
  EXPECT_EQ(number_to_serialize, buffer[0]);
}

TEST(Message, SerializeUnsignedInteger_FitsTo2Bytes) {
  int buffer_size = 64;
  int number_to_serialize = 128;

  uint8_t buffer[buffer_size];
  void *buf = &buffer;
  int buf_len = sizeof(buffer);
  message::write_unsigned_integer(&buf, &buf_len, number_to_serialize);

  EXPECT_EQ(buffer_size - 2, buf_len);
  EXPECT_EQ(buffer + 2, buf);
  EXPECT_EQ(1 << 7, buffer[0]);
  EXPECT_EQ(1, buffer[1]);
}

TEST(Message, SerializeSignedInteger_Zero) {
  int buffer_size = 64;
  int number_to_serialize = 0;

  uint8_t buffer[buffer_size];
  void *buf = &buffer;
  int buf_len = sizeof(buffer);
  message::write_signed_integer(&buf, &buf_len, number_to_serialize);

  EXPECT_EQ(buffer_size - 1, buf_len);
  EXPECT_EQ(buffer + 1, buf);
  EXPECT_EQ(number_to_serialize, buffer[0]);
}

TEST(Message, SerializeSignedInteger_FitsTo1Byte) {
  int buffer_size = 64;
  int number_to_serialize = 63;

  uint8_t buffer[buffer_size];
  void *buf = &buffer;
  int buf_len = sizeof(buffer);
  message::write_signed_integer(&buf, &buf_len, number_to_serialize);

  EXPECT_EQ(buffer_size - 1, buf_len);
  EXPECT_EQ(buffer + 1, buf);
  EXPECT_EQ(number_to_serialize, buffer[0]);
}

TEST(Message, SerializeSignedInteger_FitsTo2Bytes) {
  int buffer_size = 64;
  int number_to_serialize = 64;

  uint8_t buffer[buffer_size];
  void *buf = &buffer;
  int buf_len = sizeof(buffer);
  message::write_signed_integer(&buf, &buf_len, number_to_serialize);

  EXPECT_EQ(buffer_size - 2, buf_len);
  EXPECT_EQ(buffer + 2, buf);
  EXPECT_EQ(1 << 7, buffer[0]);
  EXPECT_EQ(1, buffer[1]);
}

TEST(Message, SerializeSignedInteger_NegativeNumber2Bytes) {
  int buffer_size = 64;
  int number_to_serialize = -64;

  uint8_t buffer[buffer_size];
  void *buf = &buffer;
  int buf_len = sizeof(buffer);
  message::write_signed_integer(&buf, &buf_len, number_to_serialize);

  EXPECT_EQ(buffer_size - 2, buf_len);
  EXPECT_EQ(buffer + 2, buf);
  EXPECT_EQ(3 << 6, buffer[0]);
  EXPECT_EQ(1, buffer[1]);
}
