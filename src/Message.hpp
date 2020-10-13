#ifndef NAMINUKAS_BRAIN_MESSAGE_H_
#define NAMINUKAS_BRAIN_MESSAGE_H_

#include "WebSocket.hpp"

namespace message {
  #define TELEMETRY_DEFINITION 0
  #define UPDATE_TELEMETRY 1

  int write_data(void **buf, int *buf_len, void *data, size_t len);
  int read_data(void **buf, int *buf_len, void *data, size_t len);

  int write_int(void **buf, int *buf_len, int value);
  int read_int(void **buf, int *buf_len, int *value);

  int write_float(void **buf, int *buf_len, float value);
  int read_float(void **buf, int *buf_len, float *value);

  int write_double(void **buf, int *buf_len, double value);

  int read_double(void **buf, int *buf_len, double *value);
}

class MessageHandler {
  public:
    void handle(WebSocketServer *server, int fd, void *payload, size_t size);
};

#endif  // NAMINUKAS_BRAIN_MESSAGE_H_
