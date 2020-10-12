#include <sys/epoll.h>
#include "robotcontrol.h"
#include "Logger.hpp"
#include "IOServer.hpp"

#define MAX_EPOLL_EVENTS 8
#define EPOLL_WAIT_MILLIS 1000

void IOServer::start() {
  int epoll_fd = epoll_create1(0);
	if (epoll_fd == -1) {
		logger::last("Can't create epoll descriptor");
    return;
	}
  while (/*rc_get_state() != EXITING*/true) {
		struct epoll_event events[MAX_EPOLL_EVENTS];
		int result = epoll_wait(epoll_fd, events, MAX_EPOLL_EVENTS, EPOLL_WAIT_MILLIS);
		for (int i = 0; i < result; i++) {
			//handle_event(&events[i]);
		}
	}
}
