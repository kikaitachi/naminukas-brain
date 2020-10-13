#include "Logger.hpp"
#include "MessageHandler.hpp"

void MessageHandler::handle(WebSocketServer *server, int fd, void *payload, size_t size) {
  logger::info("Got message from %d of %d bytes", fd, size);
}
