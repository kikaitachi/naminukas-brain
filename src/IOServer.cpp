#include <cstring>
#include <sys/epoll.h>
#include "Logger.hpp"
#include "IOServer.hpp"

#define MAX_EPOLL_EVENTS 8
#define EPOLL_WAIT_MILLIS 1000

IOServer::IOServer() {
  epoll_fd = epoll_create1(0);
	if (epoll_fd == -1) {
		logger::last("Can't create epoll descriptor");
	}
}

void IOServer::start(std::function<bool()> is_terminated) {
  while (!is_terminated()) {
		struct epoll_event events[MAX_EPOLL_EVENTS];
		int result = epoll_wait(epoll_fd, events, MAX_EPOLL_EVENTS, EPOLL_WAIT_MILLIS);
		for (int i = 0; i < result; i++) {
      int fd = events[i].data.fd;
      auto handler = fd_to_handler.find(fd);
      if (handler == fd_to_handler.end()) {
        logger::error("Got epoll event for descriptor %d which doesn't have handler", fd);
      } else {
        if (!handler->second(fd)) {
          fd_to_handler.erase(fd);
        }
      }
		}
	}
}

bool IOServer::add_handler(int fd, uint32_t events, std::function<bool(int)> handler) {
  struct epoll_event event;
  std::memset(&event, 0, sizeof(event));
	event.data.fd = fd;
	event.events = events;
	if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &event) == -1) {
		logger::last("Failed to add descriptor %d to epoll descriptor %d for events %d",
			fd, epoll_fd, events);
		return false;
	}
  fd_to_handler[fd] = handler;
	return true;
}
