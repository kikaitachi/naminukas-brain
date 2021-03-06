#include "LocomotionTail.hpp"

LocomotionTail::LocomotionTail(hardware::Kinematics& kinematics, hardware::IMU& imu)
    : Locomotion(10), kinematics(kinematics), imu(imu) {
}

std::string LocomotionTail::name() {
  return "Tail";
}

void LocomotionTail::on_start() {
  kinematics.set_joint_control_mode(hardware::Joint::left_wheel, hardware::JointControlMode::velocity, 1000);
  kinematics.set_joint_control_mode(hardware::Joint::left_ankle, hardware::JointControlMode::position, 1000, 5);
  kinematics.set_joint_control_mode(hardware::Joint::right_ankle, hardware::JointControlMode::position, 1000, 5);
  kinematics.set_joint_control_mode(hardware::Joint::right_wheel, hardware::JointControlMode::velocity, 1000);
  kinematics.set_joint_position({
    { hardware::Joint::left_ankle, initial_ankle_angle - 90 },
    { hardware::Joint::right_ankle, initial_ankle_angle + 90 },
  });
}

void LocomotionTail::on_stop() {
  // Power down motors
  kinematics.set_joint_control_mode(hardware::Joint::left_wheel, hardware::JointControlMode::off);
  kinematics.set_joint_control_mode(hardware::Joint::left_ankle, hardware::JointControlMode::off);
  kinematics.set_joint_control_mode(hardware::Joint::right_ankle, hardware::JointControlMode::off);
  kinematics.set_joint_control_mode(hardware::Joint::right_wheel, hardware::JointControlMode::off);
}

void LocomotionTail::halt() {
  kinematics.set_joint_speed({ { hardware::Joint::left_wheel, 0 }, { hardware::Joint::right_wheel, 0 } });
}

void LocomotionTail::up(bool key_down, std::set<std::string>& modifiers) {
  if (key_down) {
    kinematics.set_joint_speed({
      { hardware::Joint::left_wheel, -max_rpm },
      { hardware::Joint::right_wheel, max_rpm }
    });
  } else {
    halt();
  }
}

void LocomotionTail::down(bool key_down, std::set<std::string>& modifiers) {
  if (key_down) {
    kinematics.set_joint_speed({
      { hardware::Joint::left_wheel, max_rpm },
      { hardware::Joint::right_wheel, -max_rpm }
    });
  } else {
    halt();
  }
}

void LocomotionTail::left(bool key_down, std::set<std::string>& modifiers) {
  if (key_down) {
    if (modifiers.find("Control") != modifiers.end()) {
      kinematics.set_joint_position({
        { hardware::Joint::right_ankle, initial_ankle_angle - 50 }
      });
    } else {
      kinematics.set_joint_speed({
        { hardware::Joint::left_wheel, max_rpm },
        { hardware::Joint::right_wheel, max_rpm }
      });
    }
  } else {
    halt();
  }
}

void LocomotionTail::right(bool key_down, std::set<std::string>& modifiers) {
  if (key_down) {
    if (modifiers.find("Control") != modifiers.end()) {
      kinematics.set_joint_position({
        { hardware::Joint::right_ankle, initial_ankle_angle + 90 }
      });
    } else {
      kinematics.set_joint_speed({
        { hardware::Joint::left_wheel, -max_rpm },
        { hardware::Joint::right_wheel, -max_rpm }
      });
    }
  } else {
    halt();
  }
}
