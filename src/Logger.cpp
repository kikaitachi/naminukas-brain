#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include <string>
#include <cstring>

#include "Logger.hpp"

namespace logger {

  static level current_level = level_info;

  void set_level(level log_level) {
    current_level = log_level;
  }

  level get_level() {
    return current_level;
  }

  void log_entry(char level, const char *format, va_list argptr) {
    struct timespec now_timespec;
    clock_gettime(CLOCK_REALTIME, &now_timespec);
    time_t now_t = now_timespec.tv_sec;
    struct tm now_tm;
    localtime_r(&now_t, &now_tm);
    char buffer[1024 * 4];
    snprintf(buffer, sizeof(buffer),
      "%02d-%02d-%02d %02d:%02d:%02d.%06ld %c %s\n",
      now_tm.tm_year + 1900, now_tm.tm_mon + 1, now_tm.tm_mday,
      now_tm.tm_hour, now_tm.tm_min, now_tm.tm_sec,
      now_timespec.tv_nsec / 1000, level, format);
    vfprintf(stderr, buffer, argptr);
  }

  void debug(std::string format, ...) {
    if (current_level <= level_debug) {
      va_list argptr;
      va_start(argptr, format);
      log_entry('D', format.c_str(), argptr);
      va_end(argptr);
    }
  }

  void warn(std::string format, ...) {
    if (current_level <= level_warn) {
      va_list argptr;
      va_start(argptr, format);
      log_entry('W', format.c_str(), argptr);
      va_end(argptr);
    }
  }

  void error(std::string format, ...) {
    if (current_level <= level_error) {
      va_list argptr;
      va_start(argptr, format);
      log_entry('E', format.c_str(), argptr);
      va_end(argptr);
    }
  }

  void info(std::string format, ...) {
    if (current_level <= level_info) {
      va_list argptr;
      va_start(argptr, format);
      log_entry('I', format.c_str(), argptr);
      va_end(argptr);
    }
  }

  void last(std::string format, ...) {
    std::string error(strerror(errno));
    format += ": " + error;
    va_list argptr;
    va_start(argptr, format);
    log_entry('L', format.c_str(), argptr);
    va_end(argptr);
  }

}  // namespace logger
