#include <fstream>
#include <iomanip>
#include <memory>
#include <sstream>
#include <thread>
#include <utility>
#include <vector>

#include "robotcontrol.h"

#include "Logger.hpp"
#include "SystemTelemetry.hpp"

// From https://blog.ampow.com/lipo-voltage-chart/
const std::vector<std::pair<float, int>> voltage_to_percentage({
  {4.2, 100},
  {4.15, 95},
  {4.11, 90},
  {4.08, 85},
  {4.02, 80},
  {3.98, 75},
  {3.95, 70},
  {3.91, 65},
  {3.87, 60},
  {3.85, 55},
  {3.84, 50},
  {3.82, 45},
  {3.80, 40},
  {3.79, 35},
  {3.77, 30},
  {3.75, 25},
  {3.73, 20},
  {3.71, 15},
  {3.69, 10},
  {3.61, 5},
  {3.27, 0}
});

static std::string format_battery(float voltage, int cells) {
  int percentage = 0;
  for (auto pair : voltage_to_percentage) {
    if (pair.first <= voltage / cells) {
      percentage = pair.second;
      break;
    }
  }
  std::stringstream stream;
  stream << std::fixed << std::setprecision(3) << voltage << "V, " << percentage << "%";
  return stream.str();
}

static std::string format_load(struct sysinfo& system_info) {
  float load_avg_1m = system_info.loads[0] * 1.f / (1 << SI_LOAD_SHIFT);
  float load_avg_5m = system_info.loads[1] * 1.f / (1 << SI_LOAD_SHIFT);
  float load_avg_15m = system_info.loads[2] * 1.f / (1 << SI_LOAD_SHIFT);
  std::stringstream stream;
  stream << std::fixed << std::setprecision(2)
    << load_avg_1m << ", " << load_avg_5m << ", " << load_avg_15m;
  return stream.str();
}

SystemTelemetry::SystemTelemetry(telemetry::Items& telemetryItems, std::function<bool()> is_terminated) {
  struct utsname system_name;
  if (uname(&system_name) == -1) {
    logger::last("Can't determine system name");
  }

  // Assume that BeagleBone Blue is connected to 2S LiPo, otherwise use 3S for simulation
  battery_cells = rc_adc_init() == 0 ? 2 : 3;

  std::shared_ptr<telemetry::Item> machine = std::make_shared<telemetry::ItemString>(
    telemetry::ROOT_ITEM_ID, std::string(system_name.nodename),
    std::string(system_name.sysname) + " " + std::string(system_name.release) + " " + std::string(system_name.version) + " " + std::string(system_name.machine));
  telemetryItems.add_item(machine);

  std::shared_ptr<telemetry::ItemString> uptime = std::make_shared<telemetry::ItemString>(
    machine->getId(), "Uptime", "");
  telemetryItems.add_item(uptime);

  std::shared_ptr<telemetry::ItemString> load_average = std::make_shared<telemetry::ItemString>(
    machine->getId(), "Load avg. 1, 5, 15m", "");
  telemetryItems.add_item(load_average);

  std::shared_ptr<telemetry::ItemInt> freeMemory = std::make_shared<telemetry::ItemInt>(
    machine->getId(), "Free memory, MiB", -1);
  telemetryItems.add_item(freeMemory);

  std::shared_ptr<telemetry::ItemString> battery = std::make_shared<telemetry::ItemString>(
    machine->getId(), "Battery (min 7.4V)", "");
  telemetryItems.add_item(battery);
  std::shared_ptr<telemetry::ItemFloat> charger = std::make_shared<telemetry::ItemFloat>(
    machine->getId(), "Charger, V", 0);
  telemetryItems.add_item(charger);

  telemetryItems.add_item(std::make_shared<telemetry::ItemChoice>(
    machine->getId(), "Log level",
    std::initializer_list<std::string>{"debug", "info", "warn", "error"},
    static_cast<int>(logger::get_level()),
    [](int new_level) {
      logger::set_level((logger::level)new_level);
    }));

  std::thread update_thread([=]() {
    while (!is_terminated()) {
      struct sysinfo system_info;
      if (sysinfo(&system_info) == -1) {
        logger::last("Failed to retrieve system information");
      } else {
        int seconds = system_info.uptime % 60;
        int minutes = (system_info.uptime / 60) % 60;
        int hours = (system_info.uptime / 3600) % 24;
        int days = (system_info.uptime / 3600) / 24;
        uptime->update(std::to_string(days) + ":" + std::to_string(hours) + ":" + std::to_string(minutes) + ":" + std::to_string(seconds));

        load_average->update(format_load(system_info));
        freeMemory->update(system_info.freeram * system_info.mem_unit / (1024 * 1024));

        battery->update(format_battery(read_battery_voltage(), battery_cells));
        if (battery_cells == 2) {
          charger->update(rc_adc_dc_jack());
        }
      }
      std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
  });
  update_thread.detach();
}

SystemTelemetry::~SystemTelemetry() {
  if (battery_cells == 2) {
    rc_adc_cleanup();
  }
}

float SystemTelemetry::read_battery_voltage() {
  if (battery_cells == 2) {
    return rc_adc_batt();
  }
  std::ifstream battery_voltage_file("/sys/class/power_supply/BAT0/voltage_now");
  if (!battery_voltage_file.fail()) {
    float voltage;
    battery_voltage_file >> voltage;
    return voltage / 1000000;
  }
  return 0;
}
