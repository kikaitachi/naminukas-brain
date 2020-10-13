#include <cstring>
#include <thread>
#include <netinet/in.h>
#include "Logger.hpp"
#include "WebSocket.hpp"

WebSocketServer::WebSocketServer(int port) {
  server_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (server_fd == -1) {
    logger::last("Failed to create WebSocket server socket");
    return;
  }
  int sock_option = 1;
  if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &sock_option, sizeof(sock_option)) == -1) {
    logger::last("Can't enable SO_REUSEADDR option for WebSocket server socket");
    return;
  }
  struct sockaddr_in serv_addr;
  memset(&serv_addr, '0', sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  serv_addr.sin_port = htons(port);
  if (bind(server_fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1) {
    logger::last("Failed to bind WebSocket server socket");
    return;
  } else {
    logger::debug("WebSocket was bount to port %d", port);
  }
  if (listen(server_fd, 10) == -1) {
    logger::last("Failed to listen to WebSocket server socket");
    return;
  }
}

void WebSocketServer::add_client(int fd) {
  std::thread client_thread([&]() { handle_client(fd); });
}

void WebSocketServer::handle_client(int fd) {
  clients.insert(fd);
}
