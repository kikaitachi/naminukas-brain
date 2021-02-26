#include <cmath>
#include <vector>

#include "LocomotionPole.hpp"

LocomotionPole::LocomotionPole(hardware::Kinematics& kinematics, Model& model)
    : Locomotion(10), kinematics(kinematics), model(model) {
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

  return pose;
}

void LocomotionPole::on_start() {
  kinematics.set_joint_control_mode(hardware::Joint::left_wheel, hardware::JointControlMode::velocity, 2000);
  kinematics.set_joint_control_mode(hardware::Joint::left_ankle, hardware::JointControlMode::position);
  kinematics.set_joint_control_mode(hardware::Joint::right_ankle, hardware::JointControlMode::position);
  kinematics.set_joint_control_mode(hardware::Joint::right_wheel, hardware::JointControlMode::velocity, 2000);
  kinematics.set_joint_position({
    { hardware::Joint::left_ankle, initial_ankle_angle + 10 },
    { hardware::Joint::right_ankle, initial_ankle_angle - 10 },
  });
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
    kinematics.set_joint_speed({
      { hardware::Joint::left_wheel, -max_rpm }, { hardware::Joint::right_wheel, max_rpm }
    });
  } else {
    halt();
  }
}

void LocomotionPole::down(bool key_down, std::set<std::string>& modifiers) {
  if (key_down) {
    kinematics.set_joint_speed({
      { hardware::Joint::left_wheel, max_rpm }, { hardware::Joint::right_wheel, -max_rpm }
    });
  } else {
    halt();
  }
}

void LocomotionPole::left(bool key_down, std::set<std::string>& modifiers) {
  if (key_down) {
    kinematics.set_joint_speed({
      { hardware::Joint::left_wheel, max_rpm }, { hardware::Joint::right_wheel, max_rpm }
    });
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
