#ifndef NAMINUKAS_BRAIN_WEBSOCKET_H_
#define NAMINUKAS_BRAIN_WEBSOCKET_H_

#include <map>
#include <functional>

#include "Client.hpp"

class WebSocketServer {
  public:
    int server_fd;

    WebSocketServer(int port,
      std::function<void(WebSocketServer*, int)> on_connect,
      std::function<void(WebSocketServer*, Client*, void*, size_t)> on_binary_message);
    void accept_client();
    void sendBinary(int fd, void *data, size_t size);
    //void sendBinaryAll(void *data, size_t size);

  private:
    std::function<void(WebSocketServer*, int)> on_connect;
    std::function<void(WebSocketServer*, Client*, void*, size_t)> on_binary_message;
    std::map<int, Client*> fd_to_client;

    void handle_client(int fd);
    void sendFrame(int fd, char opcode, void *data, size_t size);
    void disconnect(int fd, bool error, std::string reason);
};

#endif  // NAMINUKAS_BRAIN_WEBSOCKET_H_
