#ifndef SRC_SYSTEMTELEMETRY_HPP_
#define SRC_SYSTEMTELEMETRY_HPP_

#include <sys/sysinfo.h>
#include <sys/utsname.h>

#include <string>

#include "Telemetry.hpp"

class SystemTelemetry {
 public:
  SystemTelemetry(
    telemetry::Items& telemetryItems,
    std::function<bool()> is_terminated);
  ~SystemTelemetry();

 private:
  int battery_cells;
  float read_battery_voltage();
};

#endif  // SRC_SYSTEMTELEMETRY_HPP_
