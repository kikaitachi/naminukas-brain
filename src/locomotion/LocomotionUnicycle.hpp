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
    void control_loop();
    void on_start();
    void on_stop();
    void up(bool key_down, std::set<std::string>& modifiers);
    void down(bool key_down, std::set<std::string>& modifiers);

  protected:
    hardware::Kinematics& kinematics;
    PIDController roll_controller;
    IMU& imu;
    std::vector<hardware::JointPosition> initial_pos;
};

#endif // NAMINUKAS_BRAIN_LOCOMOTION_UNICYCLE_H_
