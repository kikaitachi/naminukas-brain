#ifndef NAMINUKAS_BRAIN_WEBSOCKET_H_
#define NAMINUKAS_BRAIN_WEBSOCKET_H_

namespace websocket {
  /**
   * @return file description accepting client connections.
   */
  int create_server_socket(int port);

  void handle_client_connection(int fd);
};

#endif  // NAMINUKAS_BRAIN_WEBSOCKET_H_
