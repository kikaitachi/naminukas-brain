#ifndef SRC_LOCOMOTION_LOCOMOTIONUNICYCLE_HPP_
#define SRC_LOCOMOTION_LOCOMOTIONUNICYCLE_HPP_

#include <set>
#include <string>

#include "../Hardware.hpp"
#include "Locomotion.hpp"
#include "../PIDController.hpp"

/**
 * An attempt to drive on a single wheel.
 * Far from successful yet.
 */
class LocomotionUnicycle: public Locomotion {
 public:
  LocomotionUnicycle(hardware::Kinematics& kinematics, hardware::IMU& imu);
  std::string name();
  Pose control_loop(Pose pose);
  void on_start();
  void on_stop();
  void up(bool key_down, std::set<std::string>& modifiers);
  void down(bool key_down, std::set<std::string>& modifiers);

 protected:
  hardware::Kinematics& kinematics;
  PIDController<double> roll_controller;
  hardware::IMU& imu;
};

#endif  // SRC_LOCOMOTION_LOCOMOTIONUNICYCLE_HPP_
