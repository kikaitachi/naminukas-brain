#ifndef NAMINUKAS_BRAIN_ROBOT_H_
#define NAMINUKAS_BRAIN_ROBOT_H_

#include <mutex>

#include "Telemetry.hpp"

class Robot {
  public:
    Robot(telemetry::Items& telemetryItems);
    static void lockState();
    static void unlockState();

  private:
    static std::mutex stateMutex;
    telemetry::Items& telemetryItems;
    telemetry::ItemString* mode;
};

#endif  // NAMINUKAS_BRAIN_ROBOT_H_
