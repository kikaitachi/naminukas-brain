#ifndef SRC_IOSERVER_HPP_
#define SRC_IOSERVER_HPP_

#include <map>
#include <functional>

class IOServer {
 public:
  IOServer();
  void start(std::function<bool()> is_terminated);
  bool add_handler(int fd, uint32_t events, std::function<bool(int)> handler);
 private:
  int epoll_fd;
  std::map<int, std::function<bool(int)>> fd_to_handler;
};

#endif  // SRC_IOSERVER_HPP_
