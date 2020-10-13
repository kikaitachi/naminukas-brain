#ifndef NAMINUKAS_BRAIN_WEBSOCKET_H_
#define NAMINUKAS_BRAIN_WEBSOCKET_H_

#include <set>

class WebSocketServer {
  public:
    int server_fd;

    WebSocketServer(int port);
    void add_client(int fd);

  private:
    std::set<int> clients;

    void handle_client(int fd);
};

#endif  // NAMINUKAS_BRAIN_WEBSOCKET_H_
