#ifndef SRC_LOCOMOTION_LOCOMOTIONSKI_HPP_
#define SRC_LOCOMOTION_LOCOMOTIONSKI_HPP_

#include <set>
#include <string>

#include "../IMU.hpp"
#include "Locomotion.hpp"

/**
 * Skiing is an automotive driving stunt where the car is driven while balanced
 * only on two wheels, either the pair on the driver side or on the passenger side.
 * See: https://en.wikipedia.org/wiki/Ski_(driving_stunt)
 */
class LocomotionSki: public Locomotion {
 public:
  LocomotionSki(hardware::Kinematics& kinematics, IMU& imu);
  std::string name();
  Pose control_loop(Pose pose);
  void on_start();
  void on_stop();
  void up(bool key_down, std::set<std::string>& modifiers);
  void down(bool key_down, std::set<std::string>& modifiers);
  void left(bool key_down, std::set<std::string>& modifiers);
  void right(bool key_down, std::set<std::string>& modifiers);

 protected:
  hardware::Kinematics& kinematics;
  IMU& imu;
  const double max_rpm = 20;
  const double turn_rpm = 10;
  const double initial_ankle_angle = 360.0 / 16;
  const double max_ankle_change = 20;
  const double expected_pitch = 24;
  double prev_error;
};

#endif  // SRC_LOCOMOTION_LOCOMOTIONSKI_HPP_
