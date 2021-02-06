#include <cmath>
#include <cstring>
#include <string>
#include <endian.h>
#include "Logger.hpp"
#include "Message.hpp"

namespace message {

  int write_data(void **buf, int *buf_len, const void *data, size_t len) {
  	if (*buf_len < len) {
  		return -1;
  	}
  	memcpy(*buf, data, len);
  	*buf = (int8_t *)*buf + len;
  	*buf_len = *buf_len - len;
  	return 0;
  }

  int read_data(void **buf, int *buf_len, void *data, size_t len) {
  	if (*buf_len < len) {
  		return -1;
  	}
  	memcpy(data, *buf, len);
  	*buf = (int8_t *)*buf + len;
  	*buf_len = *buf_len - len;
  	return 0;
  }

  int read_int(void **buf, int *buf_len, int *value) {
  	int negative;
  	for (int i = 0; *buf_len > 0; ) {
  		int byte = ((int8_t *)*buf)[0];
  		if (i == 0) {
  			*value = byte & 63;
  			negative = (byte & (1 << 6)) > 0;
  			i += 6;
  		} else {
  			*value = *value | ((byte & 127) << i);
  			i += 7;
  		}
  		*buf = (int8_t *)*buf + 1;
  		*buf_len = *buf_len - 1;
  		if (byte >= 0) {
  			if (negative) {
  				*value = -*value;
  			}
  			return 0;
  		}
  	}
  	return -1;
  }

  int write_byte(void **buf, int *buf_len, uint8_t byte) {
    if (*buf_len > 0) {
      ((int8_t *)*buf)[0] = byte;
      *buf = (int8_t *)*buf + 1;
      *buf_len = *buf_len - 1;
      return 0;
    }
    return -1;
  }

  int write_unsigned_integer(void **buf, int *buf_len, uint64_t value) {
    do {
      if (write_byte(buf, buf_len, ((value > 127 ? 1 : 0) << 7) | (value & 127)) == -1) {
        return -1;
      }
      value >>= 7;
    } while (value > 0);
    return 0;
  }

  int write_signed_integer(void **buf, int *buf_len, int64_t value) {
    int sign;
    if (value < 0) {
      sign = 1 << 6;
      value = -value;
    } else {
      sign = 0;
    }
    if (write_byte(buf, buf_len, ((value > 63 ? 1 : 0) << 7) | sign | (value & 63)) == -1) {
      return -1;
    }
    value >>= 6;
    while (value > 0) {
      if (write_byte(buf, buf_len, ((value > 127 ? 1 : 0) << 7) | (value & 127)) == -1) {
        return -1;
      }
      value >>= 7;
    }
    return 0;
  }

  std::string read_string(void **buf, int *buf_len) {
    int length;
    if (read_int(buf, buf_len, &length) == -1) {
      return "";
    }
    char data[length + 1];
    read_data(buf, buf_len, data, length);
    data[length] = 0;
    return std::string(data);
  }

  int write_string(void **buf, int *buf_len, std::string value) {
    if (write_unsigned_integer(buf, buf_len, value.length()) == -1) {
      return -1;
    }
    return write_data(buf, buf_len, value.c_str(), value.length());
  }

  int write_float(void **buf, int *buf_len, float value) {
    uint32_t big_endian = htobe32(reinterpret_cast<uint32_t&>(value));
    return write_data(buf, buf_len, &big_endian, 4);
  }

  int read_float(void **buf, int *buf_len, float *value) {
    uint32_t big_endian;
    if (write_data(buf, buf_len, &big_endian, 4) == -1) {
      return -1;
    }
    big_endian = be32toh(big_endian);
    *value = reinterpret_cast<float&>(big_endian);
    return 0;
  }

  int write_double(void **buf, int *buf_len, double value) {
    uint64_t big_endian = htobe64(reinterpret_cast<uint64_t&>(value));
    return write_data(buf, buf_len, &big_endian, 8);
  }

  int read_double(void **buf, int *buf_len, double *value) {
    uint64_t big_endian;
    if (write_data(buf, buf_len, &big_endian, 8) == -1) {
      return -1;
    }
    big_endian = be64toh(big_endian);
    *value = reinterpret_cast<float&>(big_endian);
    return 0;
  }
}

MessageHandler::MessageHandler(telemetry::Items& telemetryItems) :
    telemetryItems(telemetryItems) {
};

#define MAX_OUT_MSG_SIZE 1024 * 4

void MessageHandler::send_updated_telemetry(WebSocketServer *server, Client *client) {
  char buffer[MAX_OUT_MSG_SIZE];
  for (auto &telemetry_item_id : client->changed_telemetry_item_ids) {
    std::map<int, telemetry::Item*>::iterator it = telemetryItems.id_to_item.find(telemetry_item_id);
    if (it != telemetryItems.id_to_item.end()) {
      void *buf = buffer;
      int buf_len = sizeof(buffer);
      message::write_unsigned_integer(&buf, &buf_len, message::TELEMETRY_UPDATE);
      message::write_signed_integer(&buf, &buf_len, it->second->getId());
      it->second->serialize_value(&buf, &buf_len);
      server->sendBinary(client->fd, buffer, sizeof(buffer) - buf_len);
    }
  }
  client->changed_telemetry_item_ids.clear();
}

void MessageHandler::handle(WebSocketServer *server, Client *client, void *payload, size_t size) {
  void *buf = payload;
  int buf_len = size;
  int msg_type;
  message::read_int(&buf, &buf_len, &msg_type);
  //logger::debug("Got message from %d of type %d (non decoded: %d) of %d bytes",
  //  client->fd, msg_type, (int)(((int8_t *)payload)[0]), (int)size);
  switch (msg_type) {
    case message::TELEMETRY_QUERY: {
	    send_updated_telemetry(server, client);
      break;
    }
    case message::TELEMETRY_UPDATE: {
      int item_id;
      logger::debug("Telemetry update message of length %d: %d", buf_len, *((int8_t *)buf));
      message::read_int(&buf, &buf_len, &item_id);
      std::map<int, telemetry::Item*>::iterator it = telemetryItems.id_to_item.find(item_id);
      if (it == telemetryItems.id_to_item.end()) {
        logger::warn("Received telemetry update message for item with non existing id %d", item_id);
      } else {
        if (it->second->getType() == telemetry::TYPE_ACTION) {
          it->second->deserialize_value(&buf, &buf_len);
        } else {
          logger::warn("Received telemetry update message for item %d with non action type %d", item_id, it->second->getType());
        }
      }
      break;
    }
    default:
      logger::warn("Received unsupported message type %d from %d", msg_type, client->fd);
      break;
  }
}
