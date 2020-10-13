#include <string>
#include <cstring>
#include <thread>
#include <openssl/sha.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include "Logger.hpp"
#include "WebSocket.hpp"

#define READ_BUFFER_SIZE 1024 * 4

const char* web_socket_guid = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";

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
  char* encoded_data = (char*)malloc(*encoded_length + 1);
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

WebSocketServer::WebSocketServer(int port) {
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
    logger::debug("WebSocket was bount to port %d", port);
  }
  if (listen(server_fd, 10) == -1) {
    logger::last("Failed to listen to WebSocket server socket");
    return;
  }
}

void WebSocketServer::accept_client() {
  int client_fd = accept(server_fd, (struct sockaddr*)NULL, NULL);
  logger::info("New WebSocket connection %d", client_fd);
  std::thread client_thread([=]() { handle_client(client_fd); });
  client_thread.detach();
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
  logger::info("WebSocket header (%d bytes): %s", size, header.c_str());

  // Find start of "Sec-WebSocket-Key" header value
  char* start = strnstr(buffer, "Sec-WebSocket-Key: ", size);
  if (start == NULL) {
    logger::error("WebSocket %d request doesn't contain 'Sec-WebSocket-Key' header", fd);
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

  clients.insert(fd);
}
