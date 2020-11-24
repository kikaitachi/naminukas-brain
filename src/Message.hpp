#ifndef NAMINUKAS_BRAIN_MESSAGE_H_
#define NAMINUKAS_BRAIN_MESSAGE_H_

#include <string>
#include "WebSocket.hpp"
#include "Telemetry.hpp"

namespace message {
  const int TELEMETRY_ADD = 0;
  const int TELEMETRY_UPDATE = 1;
  const int TELEMETRY_DELETE = 2;
  const int TELEMETRY_QUERY = 3;

  int write_data(void **buf, int *buf_len, const void *data, size_t len);
  int read_data(void **buf, int *buf_len, void *data, size_t len);

  int read_int(void **buf, int *buf_len, int *value);

  int write_byte(void **buf, int *buf_len, uint8_t byte);

  int write_unsigned_integer(void **buf, int *buf_len, uint64_t value);
  int write_signed_integer(void **buf, int *buf_len, int64_t value);
  int write_string(void **buf, int *buf_len, std::string value);
  int write_double(void **buf, int *buf_len, double value);
}

class MessageHandler {
  public:
    MessageHandler(telemetry::Items& telemetryItems);
    void handle(WebSocketServer *server, Client *client, void *payload, size_t size);

  private:
    telemetry::Items& telemetryItems;
};

#endif  // NAMINUKAS_BRAIN_MESSAGE_H_
