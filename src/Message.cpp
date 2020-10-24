#include <cstring>
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

  int write_int(void **buf, int *buf_len, long long value) {
  	if (*buf_len < 1) {
  		return -1;
  	}
  	int sign;
  	if (value < 0) {
  		sign = 1 << 6;
  		value = -value;
  	} else {
  		sign = 0;
  	}
  	int8_t byte = ((value > 63 ? 1 : 0) << 7) | sign | (value & 63);
  	memcpy(*buf, &byte, 1);
  	value >>= 6;
  	*buf = (int8_t *)*buf + 1;
  	*buf_len = *buf_len - 1;
  	while (value > 0) {
  		if (*buf_len < 1) {
  			return -1;
  		}
  		int8_t byte = ((value > 127 ? 1 : 0) << 7) | (value & 127);
  		memcpy(*buf, &byte, 1);
  		value >>= 7;
  		*buf = (int8_t *)*buf + 1;
  		*buf_len = *buf_len - 1;
  	}
  	return 0;
  }

  int read_int(void **buf, int *buf_len, long long *value) {
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

  int write_float(void **buf, int *buf_len, float value) {
  	return write_data(buf, buf_len, &value, 4);
  }

  int read_float(void **buf, int *buf_len, float *value) {
  	return read_data(buf, buf_len, value, 4);
  }

  int write_double(void **buf, int *buf_len, double value) {
  	return write_data(buf, buf_len, &value, 8);
  }

  int read_double(void **buf, int *buf_len, double *value) {
  	return read_data(buf, buf_len, value, 8);
  }

  int write_string(void **buf, int *buf_len, std::string value) {
    write_int(buf, buf_len, value.length());
    return write_data(buf, buf_len, value.c_str(), value.length());
  }
}

void MessageHandler::handle(WebSocketServer *server, int fd, void *payload, size_t size) {
  logger::info("Got message from %d of %d bytes", fd, size);
}
