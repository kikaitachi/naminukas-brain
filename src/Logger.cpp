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
    fprintf(stderr, "%02d-%02d-%02d %02d:%02d:%02d.%06ld %c ",
    now_tm->tm_year + 1900, now_tm->tm_mon + 1, now_tm->tm_mday,
    now_tm->tm_hour, now_tm->tm_min, now_tm->tm_sec,
    now_timespec.tv_nsec / 1000, level);
    vfprintf(stderr, format, argptr);
    fprintf(stderr, "\n");
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
    std::string error(strerror(errno));
    format += ": " + error;
    va_list argptr;
    va_start(argptr, format);
    log_entry('L', format.c_str(), argptr);
    va_end(argptr);
  }
}
