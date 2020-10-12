#include <sys/epoll.h>
#include "robotcontrol.h"
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

void IOServer::start() {
  while (/*rc_get_state() != EXITING*/true) {
		struct epoll_event events[MAX_EPOLL_EVENTS];
		int result = epoll_wait(epoll_fd, events, MAX_EPOLL_EVENTS, EPOLL_WAIT_MILLIS);
		for (int i = 0; i < result; i++) {
      int fd = events[i].data.fd;
      auto handler = fd_to_handler.find(fd);
      if (handler == fd_to_handler.end()) {
        logger::error("Got epoll event for descriptor %d which doesn't have handler", fd);
      } else {
        handler->second(fd);
      }
		}
	}
}

bool IOServer::add_handler(int fd, uint32_t events, void (*handler)(int fd)) {
  struct epoll_event event;
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
