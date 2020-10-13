#include <string>
#include <sys/epoll.h>
#include "dynamixel_sdk/dynamixel_sdk.h"
#include "robotcontrol.h"
#include "Logger.hpp"
#include "IOServer.hpp"
#include "WebSocket.hpp"

using namespace std;

int main(int argc, const char *argv[]) {
  if (rc_adc_init()) {
    logger::error("Failed to run rc_init_adc()");
  } else {
    logger::info("Battery voltage: %f", rc_adc_batt());
  }

  dynamixel::PortHandler *portHandler = dynamixel::PortHandler::getPortHandler("/dev/ttyUSB0");
  if (portHandler->openPort()) {
    logger::info("Succeeded to open the port");
  }
  else {
    logger::error("Failed to open the port!");
  }

  IOServer ioServer;
  WebSocketServer webSocketServer(3001);
  ioServer.add_handler(webSocketServer.server_fd, EPOLLIN | EPOLLONESHOT,
    [&](int fd) { webSocketServer.accept_client(); return false; }
  );
  ioServer.start(
    []() { return rc_get_state() == EXITING; }
  );

  return 0;
}
