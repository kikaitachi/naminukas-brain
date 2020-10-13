#ifndef NAMINUKAS_BRAIN_WEBSOCKET_H_
#define NAMINUKAS_BRAIN_WEBSOCKET_H_

#include <set>
#include <functional>

class WebSocketServer {
  public:
    int server_fd;

    WebSocketServer(int port,
      std::function<void(WebSocketServer*, int)> on_connect,
      std::function<void(WebSocketServer*, int)> on_message);
    void accept_client();
    void sendBinary(int fd, void *data, size_t size);
    void sendBinaryAll(void *data, size_t size);

  private:
    std::function<void(WebSocketServer*, int)> on_connect, on_message;
    std::set<int> clients;

    void handle_client(int fd);
    void sendFrame(int fd, char opcode, void *data, size_t size);
    void disconnect(int fd, std::string reason);
};

#endif  // NAMINUKAS_BRAIN_WEBSOCKET_H_
