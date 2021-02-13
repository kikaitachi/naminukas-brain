#ifndef SRC_SYSTEMTELEMETRY_HPP_
#define SRC_SYSTEMTELEMETRY_HPP_

#include <sys/sysinfo.h>
#include <sys/utsname.h>

#include "Telemetry.hpp"

class SystemTelemetry {
 public:
  SystemTelemetry(
    telemetry::Items& telemetryItems,
    std::function<bool()> is_terminated);
  ~SystemTelemetry();

 private:
  bool battery_supported;
};

#endif  // SRC_SYSTEMTELEMETRY_HPP_
