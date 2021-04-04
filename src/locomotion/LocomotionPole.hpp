#ifndef SRC_LOCOMOTION_LOCOMOTIONPOLE_HPP_
#define SRC_LOCOMOTION_LOCOMOTIONPOLE_HPP_

#include <set>
#include <string>
#include <vector>

#include "Locomotion.hpp"
#include "TraitTilting.hpp"
#include "../Camera.hpp"
#include "../Model.hpp"

class LocomotionPole: public Locomotion, public TraitTilting {
 public:
  LocomotionPole(hardware::Kinematics& kinematics, Model& model, PointCloud& camera, hardware::IMU& imu);
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
  Model& model;
  PointCloud& camera;
  hardware::IMU& imu;
  const double max_rpm = 20;

  // Determine experimentally
  const double wheel_radius = 0.045;
  const double distance_between_wheels = 0.213;

  // Calibration results
  double yaw_to_wheel = 1;

  std::vector<hardware::JointPosition> previous_angles;

  void halt();
  void calibrate();
  void turn(double degrees);
};

#endif  // SRC_LOCOMOTION_LOCOMOTIONPOLE_HPP_
