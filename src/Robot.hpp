#ifndef NAMINUKAS_BRAIN_ROBOT_H_
#define NAMINUKAS_BRAIN_ROBOT_H_

#include <mutex>

#include "Hardware.hpp"
#include "Telemetry.hpp"

class Robot {
  public:
    Robot(telemetry::Items& telemetryItems, hardware::Kinematics& kinematics);
    static void lockState();
    static void unlockState();

  private:
    static std::mutex stateMutex;
    hardware::Kinematics& kinematics;
    telemetry::Items& telemetryItems;
    telemetry::ItemString* mode;
};

#endif  // NAMINUKAS_BRAIN_ROBOT_H_
