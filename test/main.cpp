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

int main(int argc, char *argv[]) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
