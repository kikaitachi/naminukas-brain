#include <string>
#include <sys/epoll.h>
#include "dynamixel_sdk/dynamixel_sdk.h"
#include "robotcontrol.h"
#include "Logger.hpp"
#include "IOServer.hpp"
#include "WebSocket.hpp"
#include "Message.hpp"

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

  MessageHandler messageHandler;
  IOServer ioServer;
  WebSocketServer webSocketServer(3001,
    [&](WebSocketServer* server, int fd) {
      int test = 3;
      server->sendBinary(fd, &test, 4);
    },
    [&](WebSocketServer* server, int fd, void *payload, size_t size) {
      messageHandler.handle(server, fd, payload, size);
    }
  );
  ioServer.add_handler(webSocketServer.server_fd, EPOLLIN,
    [&](int fd) {
      webSocketServer.accept_client();
      return true;
    }
  );
  ioServer.start(
    []() { return rc_get_state() == EXITING; }
  );

  return 0;
}
