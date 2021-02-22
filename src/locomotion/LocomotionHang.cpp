#include "LocomotionHang.hpp"

LocomotionHang::LocomotionHang(hardware::Kinematics& kinematics)
    : Locomotion(50), kinematics(kinematics) {
}

std::string LocomotionHang::name() {
  return "Hang";
}

Pose LocomotionHang::control_loop(Pose pose) {
  // Return original pose. Wheel odometry might be not
  // possible to calculate in this mode of locomotion.
  return pose;
}

void LocomotionHang::on_start() {
  kinematics.set_joint_control_mode(hardware::Joint::left_wheel, hardware::JointControlMode::position);
  kinematics.set_joint_control_mode(hardware::Joint::left_ankle, hardware::JointControlMode::position);
  kinematics.set_joint_control_mode(hardware::Joint::right_ankle, hardware::JointControlMode::position);
  kinematics.set_joint_control_mode(hardware::Joint::right_wheel, hardware::JointControlMode::position);
  kinematics.set_joint_position({
    { hardware::Joint::left_ankle, initial_ankle_angle },
    { hardware::Joint::right_ankle, initial_ankle_angle }
  });
}

void LocomotionHang::on_stop() {
  kinematics.set_joint_control_mode(hardware::Joint::left_wheel, hardware::JointControlMode::off);
  kinematics.set_joint_control_mode(hardware::Joint::left_ankle, hardware::JointControlMode::off);
  kinematics.set_joint_control_mode(hardware::Joint::right_ankle, hardware::JointControlMode::off);
  kinematics.set_joint_control_mode(hardware::Joint::right_wheel, hardware::JointControlMode::off);
}
