#ifndef SRC_WEBSOCKET_HPP_
#define SRC_WEBSOCKET_HPP_

#include <map>
#include <memory>
#include <functional>
#include <string>

#include "Client.hpp"
#include "Telemetry.hpp"

class WebSocketServer {
 public:
  int server_fd;
  std::map<int, Client*> fd_to_client;

  WebSocketServer(telemetry::Items& telemetryItems, int port,
    std::function<void(WebSocketServer*, int)> on_connect,
    std::function<void(WebSocketServer*, Client*, void*, size_t)> on_message);
  void accept_client();
  void sendBinary(int fd, void *data, size_t size);

 private:
  telemetry::Items& telemetryItems;
  std::shared_ptr<telemetry::ItemInt> clientCount;
  std::function<void(WebSocketServer*, int)> on_connect;
  std::function<void(WebSocketServer*, Client*, void*, size_t)> on_message;

  void handle_client(int fd);
  void sendFrame(int fd, char opcode, void *data, size_t size);
  void disconnect(int fd, bool error, std::string reason);
};

#endif  // SRC_WEBSOCKET_HPP_
