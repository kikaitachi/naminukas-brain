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
  ioServer.add_handler(
    websocket::create_server_socket(3001), EPOLLIN,
    websocket::handle_client_connection);
  ioServer.start();

  return 0;
}
