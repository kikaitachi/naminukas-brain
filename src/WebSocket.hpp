#ifndef NAMINUKAS_BRAIN_WEBSOCKET_H_
#define NAMINUKAS_BRAIN_WEBSOCKET_H_

#include <set>

class WebSocketServer {
  public:
    int server_fd;
    std::set<int> clients;

    WebSocketServer(int port);
    void add_client(int fd);
};

#endif  // NAMINUKAS_BRAIN_WEBSOCKET_H_
