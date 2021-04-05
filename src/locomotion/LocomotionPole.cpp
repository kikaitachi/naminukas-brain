#include <robotcontrol.h>

#include <cmath>
#include <set>
#include <string>
#include <vector>

#include "LocomotionPole.hpp"

static std::set<std::string> no_modifiers;

LocomotionPole::LocomotionPole(hardware::Kinematics& kinematics, Model& model, PointCloud& camera, hardware::IMU& imu)
    : Locomotion(10), TraitTilting(kinematics), kinematics(kinematics), model(model), camera(camera), imu(imu) {
}

std::string LocomotionPole::name() {
  return "Pole";
}

Pose LocomotionPole::control_loop(Pose pose) {
  std::vector<hardware::JointPosition> current_angles = kinematics.get_joint_position({
    hardware::Joint::left_wheel,
    hardware::Joint::right_wheel
  });
  if (current_angles.size() == 2) {
    double dist_left = (current_angles[0].degrees - previous_angles[0].degrees) * 2 * M_PI * wheel_radius / 360;
    double dist_right = (current_angles[1].degrees - previous_angles[1].degrees) * 2 * M_PI * wheel_radius / 360;
    double dist = (dist_left + dist_right) / 2;
    double angle = (dist_left - dist_right) / distance_between_wheels;
    pose.location.x += dist * sin(angle);
    pose.location.y += dist * cos(angle);
    previous_angles = current_angles;
  }
  switch (imu.get_tap_code()) {
    case 'w':
      up(true, no_modifiers);
      break;
    case 's':
      down(true, no_modifiers);
      break;
    case 'a':
      left(true, no_modifiers);
      break;
    case 'd':
      right(true, no_modifiers);
      break;
  }

  return pose;
}

void LocomotionPole::on_start() {
  kinematics.set_joint_control_mode(hardware::Joint::left_wheel, hardware::JointControlMode::velocity, 2000);
  kinematics.set_joint_control_mode(hardware::Joint::left_ankle, hardware::JointControlMode::position);
  kinematics.set_joint_control_mode(hardware::Joint::right_ankle, hardware::JointControlMode::position);
  kinematics.set_joint_control_mode(hardware::Joint::right_wheel, hardware::JointControlMode::velocity, 2000);
  tilt_angle = 10;
  tilt();
  previous_angles = kinematics.get_joint_position({
    hardware::Joint::left_wheel,
    hardware::Joint::right_wheel
  });
}

void LocomotionPole::on_stop() {
  kinematics.set_joint_control_mode(hardware::Joint::left_wheel, hardware::JointControlMode::off);
  kinematics.set_joint_control_mode(hardware::Joint::left_ankle, hardware::JointControlMode::off);
  kinematics.set_joint_control_mode(hardware::Joint::right_ankle, hardware::JointControlMode::off);
  kinematics.set_joint_control_mode(hardware::Joint::right_wheel, hardware::JointControlMode::off);
}

void LocomotionPole::halt() {
  kinematics.set_joint_speed({ { hardware::Joint::left_wheel, 0 }, { hardware::Joint::right_wheel, 0 } });
}

void LocomotionPole::up(bool key_down, std::set<std::string>& modifiers) {
  if (key_down) {
    if (modifiers.find("Control") != modifiers.end()) {
      tilt_angle += 5;
      tilt();
    } else {
      kinematics.set_joint_speed({
        { hardware::Joint::left_wheel, -max_rpm }, { hardware::Joint::right_wheel, max_rpm }
      });
    }
  } else {
    halt();
  }
}

void LocomotionPole::calibrate() {
  logger::info("Start calibration");
  std::vector<hardware::JointPosition> start_angles = kinematics.get_joint_position({
    hardware::Joint::left_wheel,
    hardware::Joint::right_wheel
  });
  double start_yaw = imu.get_yaw();
  const int calibrate_speed = 1;  // RPM
  kinematics.set_joint_speed({
    { hardware::Joint::left_wheel, calibrate_speed }, { hardware::Joint::right_wheel, calibrate_speed }
  });
  std::this_thread::sleep_for(std::chrono::seconds(3));
  kinematics.set_joint_speed({
    { hardware::Joint::left_wheel, 0 }, { hardware::Joint::right_wheel, 0 }
  });
  std::vector<hardware::JointPosition> end_angles = kinematics.get_joint_position({
    hardware::Joint::left_wheel,
    hardware::Joint::right_wheel
  });
  double end_yaw = imu.get_yaw();
  double delta_pos_left = end_angles[0].degrees - start_angles[0].degrees;
  double delta_pos_right = end_angles[1].degrees - start_angles[1].degrees;
  double delta_yaw = atan2(
    sin((end_yaw - start_yaw) * DEG_TO_RAD),
    cos((end_yaw - start_yaw) * DEG_TO_RAD)) * RAD_TO_DEG;
  yaw_to_wheel = (delta_pos_left + delta_pos_right) / 2 / delta_yaw;
  logger::info("End calibration");
  turn(180);
}

void LocomotionPole::turn(double degrees) {
  double delta = degrees * yaw_to_wheel;
  logger::info("Turning - delta yaw: %f, delta wheel: %f", degrees, delta);
  kinematics.set_joint_control_mode(hardware::Joint::left_wheel, hardware::JointControlMode::position, 2000);
  kinematics.set_joint_control_mode(hardware::Joint::right_wheel, hardware::JointControlMode::position, 2000);
  std::vector<hardware::JointPosition> angles = kinematics.get_joint_position({
    hardware::Joint::left_wheel,
    hardware::Joint::right_wheel
  });
  angles[0].degrees += delta;
  angles[1].degrees += delta;
  kinematics.set_joint_position(angles);
}

void LocomotionPole::down(bool key_down, std::set<std::string>& modifiers) {
  if (key_down) {
    if (modifiers.find("Control") != modifiers.end()) {
      tilt_angle -= 5;
      tilt();
    } else {
      kinematics.set_joint_speed({
        { hardware::Joint::left_wheel, max_rpm }, { hardware::Joint::right_wheel, -max_rpm }
      });
    }
  } else {
    halt();
  }
}

void LocomotionPole::left(bool key_down, std::set<std::string>& modifiers) {
  if (key_down) {
    const std::set<std::string> all_modifiers = {"Alt"};
    if (modifiers == all_modifiers) {
      calibrate();
    } else {
      kinematics.set_joint_speed({
        { hardware::Joint::left_wheel, max_rpm }, { hardware::Joint::right_wheel, max_rpm }
      });
    }
  } else {
    halt();
  }
}

void LocomotionPole::right(bool key_down, std::set<std::string>& modifiers) {
  if (key_down) {
    kinematics.set_joint_speed({
      { hardware::Joint::left_wheel, -max_rpm }, { hardware::Joint::right_wheel, -max_rpm }
    });
  } else {
    halt();
  }
}
