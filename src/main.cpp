#include <string>
#include <sys/epoll.h>
#include <sys/utsname.h>
#include "dynamixel_sdk.h"
#include "robotcontrol.h"
#include "Logger.hpp"
#include "IOServer.hpp"
#include "WebSocket.hpp"
#include "Message.hpp"
#include "Telemetry.hpp"

using namespace std;

#define MAX_MESSAGE_SIZE 64 * 1024

telemetry::Items telemetryItems;

void create_telemetry_definitions() {
  struct utsname buf;
  if (uname(&buf) == -1) {
    logger::last("Can't determine system name");
    return;
  }
  telemetryItems.add(new telemetry::ItemString(telemetry::ROOT_ITEM_ID,
    string(buf.nodename), string(buf.release) + " " + string(buf.version)));
}

void send_telemetry_definitions(WebSocketServer* server, int fd) {
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

  create_telemetry_definitions();

  MessageHandler messageHandler;
  IOServer ioServer;
  int port;
  if (const char* env_port = std::getenv("PORT")) {
    port = atoi(env_port);
  } else {
    port = 3001;
  }
  WebSocketServer webSocketServer(port,
    &send_telemetry_definitions,
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
