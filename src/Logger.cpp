#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <cstring>
#include <time.h>
#include <unistd.h>

namespace logger {

  void log_entry(char level, const char *format, va_list argptr) {
  	struct timespec now_timespec;
  	clock_gettime(CLOCK_REALTIME, &now_timespec);
  	time_t now_t = now_timespec.tv_sec;
  	struct tm *now_tm = localtime(&now_t);
  	printf("%02d-%02d-%02d %02d:%02d:%02d.%06ld \033[1m%c \033[0m",
  	now_tm->tm_year + 1900, now_tm->tm_mon + 1, now_tm->tm_mday,
  	now_tm->tm_hour, now_tm->tm_min, now_tm->tm_sec,
  	now_timespec.tv_nsec / 1000, level);
  	if (level == 'E') {
  		printf("\033[31m");
  	}
  	vprintf(format, argptr);
  	printf("\033[0m \n");
  }

  void debug(std::string format, ...) {
  	va_list argptr;
  	va_start(argptr, format);
  	log_entry('D', format.c_str(), argptr);
  	va_end(argptr);
  }

  void warn(std::string format, ...) {
  	va_list argptr;
  	va_start(argptr, format);
  	log_entry('W', format.c_str(), argptr);
  	va_end(argptr);
  }

  void error(std::string format, ...) {
  	va_list argptr;
  	va_start(argptr, format);
  	log_entry('E', format.c_str(), argptr);
  	va_end(argptr);
  }

  void info(std::string format, ...) {
  	va_list argptr;
  	va_start(argptr, format);
  	log_entry('I', format.c_str(), argptr);
  	va_end(argptr);
  }

  void last(std::string format, ...) {
  	char buffer[1024 * 4];
  	snprintf(buffer, sizeof(buffer), "%s: %s", format.c_str(), strerror(errno));
  	va_list argptr;
  	va_start(argptr, format);
  	log_entry('L', buffer, argptr);
  	va_end(argptr);
  }
}
