#include <string>
#include <thread>

#include "robotcontrol.h"

#include "Logger.hpp"
#include "SystemTelemetry.hpp"

SystemTelemetry::SystemTelemetry(telemetry::Items& telemetryItems, std::function<bool()> is_terminated) {
  struct utsname system_name;
  if (uname(&system_name) == -1) {
    logger::last("Can't determine system name");
  }

  battery_supported = rc_adc_init() == 0;

  telemetry::Item* machine = telemetryItems.add_item(new telemetry::ItemString(
    telemetry::ROOT_ITEM_ID, std::string(system_name.nodename),
    std::string(system_name.release) + " " + std::string(system_name.version)));

  telemetry::ItemString* uptime = new telemetry::ItemString(
    machine->getId(), "Uptime", "");
  telemetryItems.add_item(uptime);

  telemetry::ItemString* load_average = new telemetry::ItemString(
    machine->getId(), "Load average", "");
  telemetryItems.add_item(load_average);

  telemetry::ItemString* freeMemory = new telemetry::ItemString(
    machine->getId(), "Free memory, MiB", "");
  telemetryItems.add_item(freeMemory);

  telemetry::ItemString* battery = new telemetry::ItemString(
    machine->getId(), "Battery, V", "");
  telemetryItems.add_item(battery);
  telemetry::ItemString* charger = new telemetry::ItemString(
    machine->getId(), "Charger, V", "");
  telemetryItems.add_item(charger);

  std::thread update_thread([=]() {
    while (!is_terminated()) {
      struct sysinfo system_info;
      if (sysinfo(&system_info) == -1) {
        logger::last("Failed to retrieve system information");
      } else {
        long seconds = system_info.uptime % 60;
        long minutes = (system_info.uptime / 60) % 60;
        long hours = (system_info.uptime / 3600) % 24;
        long days = (system_info.uptime / 3600) / 24;
        uptime->update(std::to_string(days) + ":" + std::to_string(hours) + ":" + std::to_string(minutes) + ":" + std::to_string(seconds));

        float load_avg_1m = system_info.loads[0] * 1.f / (1 << SI_LOAD_SHIFT);
        float load_avg_5m = system_info.loads[1] * 1.f / (1 << SI_LOAD_SHIFT);
        float load_avg_15m = system_info.loads[2] * 1.f / (1 << SI_LOAD_SHIFT);
        load_average->update(std::to_string(load_avg_1m) + ", " + std::to_string(load_avg_5m) + ", " + std::to_string(load_avg_15m));

        freeMemory->update(std::to_string((system_info.freeram * system_info.mem_unit) / (1024.0 * 1024)));

        if (battery_supported) {
          battery->update(std::to_string(rc_adc_batt()));
          charger->update(std::to_string(rc_adc_dc_jack()));
        }
      }
      std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
  });
  update_thread.detach();
}

SystemTelemetry::~SystemTelemetry() {
  if (battery_supported) {
    rc_adc_cleanup();
    logger::debug("Cleaning up ADC");
  }
}
