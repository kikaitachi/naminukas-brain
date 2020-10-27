#include <string>
#include <signal.h>
#include <sys/epoll.h>
#include "dynamixel_sdk.h"
#include "Logger.hpp"
#include "IOServer.hpp"
#include "WebSocket.hpp"
#include "Message.hpp"
#include "Telemetry.hpp"
#include "SystemTelemetry.hpp"

using namespace std;

#define DAFAULT_PORT 3001
#define MAX_MESSAGE_SIZE 64 * 1024

static bool terminated = false;

static bool is_terminated() {
  return terminated;
}

/**
 * Interrupt handler to catch Ctrl+C.
 */
static void signal_handler(int signum) {
  switch (signum) {
    case SIGINT:
      terminated = true;
      break;
  }
}

void send_telemetry_definitions(telemetry::Items &telemetryItems, WebSocketServer* server, int fd) {
  for (const auto & [id, item] : telemetryItems.id_to_item) {
    char message[MAX_MESSAGE_SIZE];
    void *buf = &message;
    int buf_len = MAX_MESSAGE_SIZE;
    message::write_int(&buf, &buf_len, message::TELEMETRY_ADD);
    item->serialize_definition(&buf, &buf_len);
    server->sendBinary(fd, message, MAX_MESSAGE_SIZE - buf_len);
  }
}

int main(int argc, const char *argv[]) {
  dynamixel::PortHandler *portHandler = dynamixel::PortHandler::getPortHandler("/dev/ttyUSB0");
  if (portHandler->openPort()) {
    logger::info("Succeeded to open the port");
  }
  else {
    logger::error("Failed to open the port!");
  }

  signal(SIGINT, signal_handler);

  telemetry::Items telemetryItems;
  SystemTelemetry systemTelemetry(telemetryItems, &is_terminated);

  MessageHandler messageHandler(telemetryItems);
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
    [&](WebSocketServer* server, Client* client, void *payload, size_t size) {
      messageHandler.handle(server, client, payload, size);
    }
  );
  ioServer.add_handler(webSocketServer.server_fd, EPOLLIN,
    [&](int fd) {
      webSocketServer.accept_client();
      return true;
    }
  );

  telemetryItems.add_change_listener([&](telemetry::Item& item) {
    for (const auto & [fd, client] : webSocketServer.fd_to_client) {
      client->changed_telemetry_item_ids.insert(item.getId());
    }
  });

  ioServer.start(&is_terminated);

  logger::info("Gracefull shutdown");

  return 0;
}
