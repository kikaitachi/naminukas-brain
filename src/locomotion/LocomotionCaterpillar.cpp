#include <cmath>

#include "LocomotionCaterpillar.hpp"

#define TILT_ANGLE 15

LocomotionCaterpillar::LocomotionCaterpillar(hardware::Kinematics& kinematics)
    : Locomotion(100), kinematics(kinematics) {
}

std::string LocomotionCaterpillar::name() {
  return "Caterpillar";
}

Pose LocomotionCaterpillar::control_loop(Pose pose) {
  if (control_loop_index % 60 == 0) {
    direction_left = -direction_left;
    kinematics.set_joint_position({
      { hardware::Joint::left_ankle, initial_ankle_angle + TILT_ANGLE * direction_left },
      { hardware::Joint::left_wheel, initial_pos[0].degrees + TILT_ANGLE * direction_left }
    });
  }
  if ((control_loop_index + 15) % 60 == 0) {
    direction_right = -direction_right;
    kinematics.set_joint_position({
      { hardware::Joint::right_ankle, initial_ankle_angle + TILT_ANGLE * direction_right },
      { hardware::Joint::right_wheel, initial_pos[1].degrees + TILT_ANGLE * direction_right }
    });
  }
  // Return original pose. Wheel odometry might be not
  // possible to calculate in this mode of locomotion.
  return pose;
}

void LocomotionCaterpillar::on_start() {
  kinematics.set_joint_control_mode(hardware::Joint::left_wheel, hardware::JointControlMode::time, 0, 0, 300, 600);
  kinematics.set_joint_control_mode(hardware::Joint::left_ankle, hardware::JointControlMode::time, 0, 0, 300, 600);
  kinematics.set_joint_control_mode(hardware::Joint::right_ankle, hardware::JointControlMode::time, 0, 0, 300, 600);
  kinematics.set_joint_control_mode(hardware::Joint::right_wheel, hardware::JointControlMode::time, 0, 0, 300, 600);
  kinematics.set_joint_position({
    { hardware::Joint::left_ankle, initial_ankle_angle },
    { hardware::Joint::right_ankle, initial_ankle_angle }
  });
  initial_pos = kinematics.get_joint_position({
    hardware::Joint::left_wheel, hardware::Joint::right_wheel
  });
  direction_left = direction_right = 0;
}

void LocomotionCaterpillar::on_stop() {
  kinematics.set_joint_control_mode(hardware::Joint::left_wheel, hardware::JointControlMode::off);
  kinematics.set_joint_control_mode(hardware::Joint::left_ankle, hardware::JointControlMode::off);
  kinematics.set_joint_control_mode(hardware::Joint::right_ankle, hardware::JointControlMode::off);
  kinematics.set_joint_control_mode(hardware::Joint::right_wheel, hardware::JointControlMode::off);
}

void LocomotionCaterpillar::up(bool key_down, std::set<std::string>& modifiers) {
  if (key_down) {
    direction_left = direction_right = 1;
  } else {
    direction_left = direction_right = 0;
  }
}

void LocomotionCaterpillar::down(bool key_down, std::set<std::string>& modifiers) {
  if (key_down) {
    direction_left = 1;
    direction_right = -1;
  } else {
    direction_left = direction_right = 0;
  }
}
