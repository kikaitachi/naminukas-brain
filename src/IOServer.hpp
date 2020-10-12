#ifndef NAMINUKAS_BRAIN_IOSERVER_H_
#define NAMINUKAS_BRAIN_IOSERVER_H_

#include <map>

class IOServer {
  public:
    IOServer();
    void start();
    bool add_handler(int fd, uint32_t events, void (*handler)(int fd));
  private:
    int epoll_fd;
    std::map<int, void (*)(int fd)> fd_to_handler;
};

#endif  // NAMINUKAS_BRAIN_IOSERVER_H_
