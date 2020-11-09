#ifndef NAMINUKAS_BRAIN_ROBOT_H_
#define NAMINUKAS_BRAIN_ROBOT_H_

#include <mutex>

#include "Hardware.hpp"
#include "IMU.hpp"
#include "Locomotion.hpp"
#include "Telemetry.hpp"

class Robot {
  public:
    Robot(telemetry::Items& telemetryItems, IMU& imu, hardware::Kinematics& kinematics);
    static void lockState();
    static void unlockState();

  private:
    static std::mutex stateMutex;
    IMU& imu;
    hardware::Kinematics& kinematics;
    telemetry::Items& telemetryItems;
    telemetry::ItemString* mode;
    Locomotion* current_locomotion_mode;

    void add_locomotion(Locomotion* locomotion, std::string key);
};

#endif  // NAMINUKAS_BRAIN_ROBOT_H_
