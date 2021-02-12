#include <fstream>
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

  std::shared_ptr<telemetry::Item> machine = std::make_shared<telemetry::ItemString>(
    telemetry::ROOT_ITEM_ID, std::string(system_name.nodename),
    std::string(system_name.sysname) + " " + std::string(system_name.release) + " " + std::string(system_name.version) + " " + std::string(system_name.machine));
  telemetryItems.add_item(machine);

  std::shared_ptr<telemetry::ItemString> uptime = std::make_shared<telemetry::ItemString>(
    machine->getId(), "Uptime", "");
  telemetryItems.add_item(uptime);

  std::shared_ptr<telemetry::ItemString> load_average = std::make_shared<telemetry::ItemString>(
    machine->getId(), "Load average", "");
  telemetryItems.add_item(load_average);

  std::shared_ptr<telemetry::ItemInt> freeMemory = std::make_shared<telemetry::ItemInt>(
    machine->getId(), "Free memory, MiB", -1);
  telemetryItems.add_item(freeMemory);

  std::shared_ptr<telemetry::ItemFloat> battery = std::make_shared<telemetry::ItemFloat>(
    machine->getId(), "Battery (min 7.4), V", 0);
  telemetryItems.add_item(battery);
  std::shared_ptr<telemetry::ItemFloat> charger = std::make_shared<telemetry::ItemFloat>(
    machine->getId(), "Charger, V", 0);
  telemetryItems.add_item(charger);

  telemetryItems.add_item(std::make_shared<telemetry::ItemChoice>(
    machine->getId(), "Log level", std::initializer_list<std::string>{"debug", "info", "warn", "error"}, (int)logger::get_level(),
    [](int new_level) {
      logger::set_level((logger::level)new_level);
    }));

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

        freeMemory->update(system_info.freeram * system_info.mem_unit / (1024 * 1024));

        if (battery_supported) {
          battery->update(rc_adc_batt());
          charger->update(rc_adc_dc_jack());
        } else {
          std::ifstream battery_voltage_file("/sys/class/power_supply/BAT0/voltage_now");
          if (!battery_voltage_file.fail()) {
            float voltage;
            battery_voltage_file >> voltage;
            battery->update(voltage / 1000000);
          }
        }
      }
      std::this_thread::sleep_for(std::chrono::milliseconds(1000));
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
