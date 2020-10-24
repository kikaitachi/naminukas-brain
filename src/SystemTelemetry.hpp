#ifndef NAMINUKAS_BRAIN_SYSTEMTELEMETRY_H_
#define NAMINUKAS_BRAIN_SYSTEMTELEMETRY_H_

#include <sys/sysinfo.h>
#include <sys/utsname.h>

#include "Telemetry.hpp"

class SystemTelemetry {
  public:
    SystemTelemetry(telemetry::Items& telemetryItems, std::function<bool()> is_terminated);
    ~SystemTelemetry();

  private:
    bool battery_supported;
};

#endif  // NAMINUKAS_BRAIN_SYSTEMTELEMETRY_H_
