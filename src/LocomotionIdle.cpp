#include "LocomotionIdle.hpp"

LocomotionIdle::LocomotionIdle(hardware::Kinematics& kinematics) : kinematics(kinematics) {
}

std::string LocomotionIdle::name() {
  return "Idle";
}

void LocomotionIdle::start() {
  kinematics.set_joint_control_mode(hardware::Joint::left_wheel, hardware::JointControlMode::off);
  kinematics.set_joint_control_mode(hardware::Joint::left_ankle, hardware::JointControlMode::off);
  kinematics.set_joint_control_mode(hardware::Joint::right_ankle, hardware::JointControlMode::off);
  kinematics.set_joint_control_mode(hardware::Joint::right_wheel, hardware::JointControlMode::off);
}
