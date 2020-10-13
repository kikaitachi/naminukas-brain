#ifndef NAMINUKAS_BRAIN_MESSAGE_HANDLER_H_
#define NAMINUKAS_BRAIN_MESSAGE_HANDLER_H_

#include "WebSocket.hpp"

class MessageHandler {
  public:
    void handle(WebSocketServer *server, int fd, void *payload, size_t size);
};

#endif  // NAMINUKAS_BRAIN_MESSAGE_HANDLER_H_
