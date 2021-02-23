#include <openssl/sha.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <algorithm>
#include <stdexcept>
#include <string>
#include <cstring>
#include <thread>

#include "Logger.hpp"
#include "WebSocket.hpp"

#define OPCODE_BINARY 2
#define OPCODE_CLOSE 8

#define FINAL_FRAME 128

#define READ_BUFFER_SIZE 1024 * 4

const char* web_socket_guid = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
const std::string websocket_key_header = "sec-websocket-key";

static char encoding_table[] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
                                'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
                                'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
                                'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
                                'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
                                'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
                                'w', 'x', 'y', 'z', '0', '1', '2', '3',
                                '4', '5', '6', '7', '8', '9', '+', '/'};
static int mod_table[] = {0, 2, 1};

static char* base64_encode(const unsigned char *data, size_t data_length, size_t *encoded_length) {
  *encoded_length = 4 * ((data_length + 2) / 3);
  char* encoded_data = reinterpret_cast<char*>(malloc(*encoded_length + 1));
  if (encoded_data == NULL) {
    return NULL;
  }
  for (int i = 0, j = 0; i < data_length;) {
    int octet_a = i < data_length ? (unsigned char)data[i++] : 0;
    int octet_b = i < data_length ? (unsigned char)data[i++] : 0;
    int octet_c = i < data_length ? (unsigned char)data[i++] : 0;

    int triple = (octet_a << 0x10) + (octet_b << 0x08) + octet_c;

    encoded_data[j++] = encoding_table[(triple >> 3 * 6) & 0x3F];
    encoded_data[j++] = encoding_table[(triple >> 2 * 6) & 0x3F];
    encoded_data[j++] = encoding_table[(triple >> 1 * 6) & 0x3F];
    encoded_data[j++] = encoding_table[(triple >> 0 * 6) & 0x3F];
  }

  for (int i = 0; i < mod_table[data_length % 3]; i++) {
    encoded_data[*encoded_length - 1 - i] = '=';
  }
  encoded_data[*encoded_length] = 0;

  return encoded_data;
}

WebSocketServer::WebSocketServer(telemetry::Items& telemetryItems, int port,
    std::function<void(WebSocketServer*, int)> on_connect,
    std::function<void(WebSocketServer*, Client*, void*, size_t)> on_message) :
    telemetryItems(telemetryItems), on_connect(on_connect), on_message(on_message) {
  server_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (server_fd == -1) {
    logger::last("Failed to create WebSocket server socket");
    return;
  }
  int sock_option = 1;
  if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &sock_option, sizeof(sock_option)) == -1) {
    logger::last("Can't enable SO_REUSEADDR option for WebSocket server socket");
    return;
  }
  struct sockaddr_in serv_addr;
  memset(&serv_addr, '0', sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  serv_addr.sin_port = htons(port);
  if (bind(server_fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1) {
    logger::last("Failed to bind WebSocket server socket");
    return;
  } else {
    logger::debug("WebSocket was bound to port %d", port);
  }
  if (listen(server_fd, 10) == -1) {
    logger::last("Failed to listen to WebSocket server socket");
    return;
  }

  clientCount = std::make_shared<telemetry::ItemInt>(telemetry::ROOT_ITEM_ID, "Connected clients", 0);
  telemetryItems.add_item(clientCount);
}

void WebSocketServer::accept_client() {
  int client_fd = accept(server_fd, (struct sockaddr*)NULL, NULL);
  logger::info("New WebSocket connection %d", client_fd);
  std::thread client_thread([=]() { handle_client(client_fd); });
  client_thread.detach();
}

void WebSocketServer::sendFrame(int fd, char opcode, void *data, size_t size) {
  char header[10];
  int header_length;
  header[0] = FINAL_FRAME | opcode;
  if (size <= 125) {
    header[1] = size;
    header_length = 2;
  } else if (size <= 65535) {
    header[1] = 126;
    header[2] = (size >> 8) & 255;
    header[3] = size & 255;
    header_length = 4;
  } else {
    header[1] = 127;
    header[2] = 0;
    header[3] = 0;
    header[4] = 0;
    header[5] = 0;
    header[6] = (size >> 24) & 255;
    header[7] = (size >> 16) & 255;
    header[8] = (size >> 8) & 255;
    header[9] = size & 255;
    header_length = 10;
  }
  // TODO: check return codes, disconnect(fd, "Failed to write frame");
  send(fd, header, header_length, 0);
  send(fd, data, size, 0);
}

void WebSocketServer::sendBinary(int fd, void *data, size_t size) {
  sendFrame(fd, OPCODE_BINARY, data, size);
}

/*
 * Find the first occurrence of find in s, where the search is limited to the
 * first slen characters of s.
 */
static char * strnstr(const char *s, const char *find, size_t slen) {
    char c, sc;
    size_t len;

    if ((c = *find++) != '\0') {
        len = strlen(find);
        do {
            do {
                if (slen-- < 1 || (sc = *s++) == '\0')
                    return (NULL);
            } while (sc != c);
            if (len > slen)
                return (NULL);
        } while (strncmp(s, find, len) != 0);
        s--;
    }
    return ((char *)s);
}

void WebSocketServer::handle_client(int fd) {
  char buffer[READ_BUFFER_SIZE];
  int size = 0;
  for ( ; ; ) {
    ssize_t result = read(fd, &buffer[size], READ_BUFFER_SIZE - size);
    if (result == -1) {
      logger::last("Failed to read HTTP header for WebSocket %d", fd);
      return;
    }
    size += result;
    if (strnstr(buffer, "\r\n\r\n", size) != NULL) {
      break;
    }
    if (size == READ_BUFFER_SIZE) {
      logger::last("WebSocket %d header exceeded %d bytes", fd, READ_BUFFER_SIZE);
      return;
    }
  }
  std::string header(buffer, size);
  logger::info("WebSocket %d header (%d bytes): %s", fd, size, header.c_str());
  transform(header.begin(), header.end(), header.begin(), ::tolower);
  size_t found = header.find(websocket_key_header);
  char* start = found == std::string::npos ? NULL : buffer + found;

  // Find start of "Sec-WebSocket-Key" header value
  if (start == NULL) {
    logger::error("WebSocket %d request doesn't contain 'Sec-WebSocket-Key' header", fd);
    snprintf(buffer, sizeof(buffer),
        "HTTP/1.1 200 OK\r\nServer: naminukas\r\nContent-Type: text/plain\r\n\r\nNaminukas robot");
    write(fd, buffer, strlen(buffer));
    disconnect(fd, false, "Non WebSocket request");
    return;
  }
  start += 19;
  // Find end of "Sec-WebSocket-Key" header value
  char* end = strstr(start, "\r");
  if (end == NULL) {
    logger::error("WebSocket %d header 'Sec-WebSocket-Key' value is not properly terminated", fd);
    return;
  }
  // TODO: Check buffer overflow
  strncpy(end, web_socket_guid, strlen(web_socket_guid));
  end += strlen(web_socket_guid);
  unsigned char hash[SHA_DIGEST_LENGTH];
  SHA1((unsigned char*)start, end - start, hash); // hash now contains the 20-byte SHA-1 hash

  size_t output_length;
  char* base64_encoded = base64_encode(hash, 20, &output_length);
  snprintf(buffer, sizeof(buffer),
      "HTTP/1.1 101 Switching Protocols\r\nUpgrade: websocket\r\nConnection: Upgrade\r\nSec-WebSocket-Accept: %s\r\n\r\n",
      base64_encoded);
  free(base64_encoded);

  ssize_t result = write(fd, buffer, strlen(buffer));
  if (result == -1) {
    logger::last("Failed to write HTTP header for WebSocket %d", fd);
    return;
  }

  // Make client available
  Client client(fd);
  fd_to_client[fd] = &client;
  clientCount->update(fd_to_client.size());

  on_connect(this, fd);

  for (size = 0; ; ) {
    result = read(fd, &buffer[size], READ_BUFFER_SIZE - size);
    if (result == -1) {
      disconnect(fd, true, "failed to read frame");
      break;
    } else if (result == 0) {
      disconnect(fd, false, "read returned 0 bytes, assuming socked closed");
      break;
    }
    size += result;
    logger::debug("WebSocket: read %d bytes from %d", (int)result, fd);

    while (size >= 6) {
      int opcode = buffer[0] & 0x0f;
      if (opcode == OPCODE_CLOSE) {
        disconnect(fd, false, "closed by control frame");
        goto finish;
      }
      if ((buffer[0] & FINAL_FRAME) == 0) {
        logger::warn("Fragmented frame received");
      }
      uint64_t data_length = buffer[1] & 127;
      size_t header_length;
      if (data_length == 126) {
       header_length = 8;
       data_length = buffer[2] << 8 | buffer[3];
      } else if (data_length == 127) {
        header_length = 14;
        data_length = (uint64_t)buffer[2] << 56 | (uint64_t)buffer[3] << 48 |
          (uint64_t)buffer[4] << 40 | (uint64_t)buffer[5] << 32 |
          buffer[6] << 24 | buffer[7] << 16 |
          buffer[7] << 8 | buffer[9];
      } else {
        header_length = 6;
      }
      size_t frame_length = data_length + header_length;

      if (size >= frame_length) {
        char mask[4];
        mask[0] = buffer[header_length - 4];
        mask[1] = buffer[header_length - 3];
        mask[2] = buffer[header_length - 2];
        mask[3] = buffer[header_length - 1];

        for (int i = 0; i < data_length; i++) {
          buffer[i + header_length] ^= mask[i % 4];
        }

        try {
          on_message(this, &client, buffer + header_length, data_length);
        } catch (const std::exception& e) {
          std::string payload = "";
          for (int i = 0; i < frame_length; i++) {
            if (payload.size() != 0) {
              payload += ", ";
            }
            payload += std::to_string(buffer[i]);
          }
          logger::error("Failed to handle message for socket %d with payload %s: %s", fd, payload.c_str(), e.what());
        }
        memmove(buffer, buffer + frame_length, size - frame_length);
        size -= frame_length;
      } else {
        break;
      }
    }
  }
finish:
  fd_to_client.erase(fd);
  clientCount->update(fd_to_client.size());
}

void WebSocketServer::disconnect(int fd, bool error, std::string reason) {
  close(fd);
  if (error) {
    logger::last("Disconnecting WebSocket %d: %s", fd, reason.c_str());
  } else {
    logger::info("Disconnecting WebSocket %d: %s", fd, reason.c_str());
  }
}
