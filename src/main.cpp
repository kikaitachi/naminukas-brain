#include <string>
#include <sys/epoll.h>
#include "dynamixel_sdk.h"
#include "robotcontrol.h"
#include "Logger.hpp"
#include "IOServer.hpp"
#include "WebSocket.hpp"
#include "Message.hpp"
#include "Telemetry.hpp"
#include "SystemTelemetry.hpp"

using namespace std;

#define DAFAULT_PORT 3001
#define MAX_MESSAGE_SIZE 64 * 1024

void send_telemetry_definitions(telemetry::Items &telemetryItems, WebSocketServer* server, int fd) {
  for (const auto & [id, item] : telemetryItems.id_to_item) {
    char message[MAX_MESSAGE_SIZE];
    void *buf = &message;
    int buf_len = MAX_MESSAGE_SIZE;
    message::write_int(&buf, &buf_len, message::TELEMETRY_DEFINITION);
    item->serialize_definition(&buf, &buf_len);
    server->sendBinary(fd, message, MAX_MESSAGE_SIZE - buf_len);
  }
}

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

  telemetry::Items telemetryItems;
  SystemTelemetry systemTelemetry(telemetryItems);

  MessageHandler messageHandler;
  IOServer ioServer;
  int port;
  if (const char* env_port = std::getenv("PORT")) {
    port = atoi(env_port);
  } else {
    port = DAFAULT_PORT;
  }
  WebSocketServer webSocketServer(port,
    [&](WebSocketServer* server, int fd) {
      send_telemetry_definitions(telemetryItems, server, fd);
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
