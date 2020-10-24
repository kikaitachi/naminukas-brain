#ifndef NAMINUKAS_BRAIN_SYSTEMTELEMETRY_H_
#define NAMINUKAS_BRAIN_SYSTEMTELEMETRY_H_

#include <sys/sysinfo.h>
#include <sys/utsname.h>

#include "Telemetry.hpp"

class SystemTelemetry {
  public:
    SystemTelemetry(telemetry::Items& telemetryItems);
};

#endif  // NAMINUKAS_BRAIN_SYSTEMTELEMETRY_H_
