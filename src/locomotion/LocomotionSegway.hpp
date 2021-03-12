#ifndef SRC_LOCOMOTION_LOCOMOTIONSEGWAY_HPP_
#define SRC_LOCOMOTION_LOCOMOTIONSEGWAY_HPP_

#include <set>
#include <string>
#include <vector>

#include "../IMU.hpp"
#include "Locomotion.hpp"
#include "../PIDController.hpp"

/**
 * Segway like balancing on two wheels.
 */
class LocomotionSegway: public Locomotion {
 public:
  LocomotionSegway(hardware::Kinematics& kinematics, IMU& imu);
  std::string name();
  Pose control_loop(Pose pose);
  void on_start();
  void on_stop();
  void up(bool key_down, std::set<std::string>& modifiers);
  void down(bool key_down, std::set<std::string>& modifiers);
  void left(bool key_down, std::set<std::string>& modifiers);
  void right(bool key_down, std::set<std::string>& modifiers);
  void stop();
 protected:
  hardware::Kinematics& kinematics;
  IMU& imu;
  PIDController<double> speed_controller;
  PIDController<double> pitch_controller;
  double pos_speed;
  double left_turn_speed, right_turn_speed;
  double goal_rpm, rpm_avg;
  double pitch;
  int sidestep_direction_left, sidestep_direction_right;
  int control_loop_iteration;
};

#endif  // SRC_LOCOMOTION_LOCOMOTIONSEGWAY_HPP_
