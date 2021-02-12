#ifndef NAMINUKAS_BRAIN_LOCOMOTION_UNICYCLE_H_
#define NAMINUKAS_BRAIN_LOCOMOTION_UNICYCLE_H_

#include "../IMU.hpp"
#include "Locomotion.hpp"
#include "../PIDController.hpp"

/**
 * An attempt to drive on a single wheel.
 * Far from successful yet.
 */
class LocomotionUnicycle: public Locomotion {
  public:
    LocomotionUnicycle(hardware::Kinematics& kinematics, IMU& imu);
    std::string name();
    Pose control_loop(Pose pose);
    void on_start();
    void on_stop();
    void up(bool key_down, std::set<std::string>& modifiers);
    void down(bool key_down, std::set<std::string>& modifiers);

  protected:
    hardware::Kinematics& kinematics;
    PIDController roll_controller;
    IMU& imu;
};

#endif // NAMINUKAS_BRAIN_LOCOMOTION_UNICYCLE_H_
