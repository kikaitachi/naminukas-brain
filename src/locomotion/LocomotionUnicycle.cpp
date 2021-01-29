#include "LocomotionUnicycle.hpp"

#define ANGLE 50

LocomotionUnicycle::LocomotionUnicycle(hardware::Kinematics& kinematics, IMU& imu)
    : Locomotion(200), roll_controller(2, 0, 0, 20, -3, 3), kinematics(kinematics), imu(imu) {
}

std::string LocomotionUnicycle::name() {
  return "Unicycle";
}

void LocomotionUnicycle::control_loop() {
  float roll = imu.get_roll();
  float goal_roll = -37;
  float roll_input = roll_controller.input(roll, goal_roll);

  kinematics.set_joint_position({
    { hardware::Joint::left_ankle, initial_ankle_angle - ANGLE - roll_input },
    { hardware::Joint::left_wheel, - roll_input }
  });
  logger::debug("roll: %f, angle: %f, fitness: %f", roll, ANGLE + roll_input, roll_controller.get_fitness());
}

void LocomotionUnicycle::on_start() {
  kinematics.set_joint_control_mode(hardware::Joint::left_wheel, hardware::JointControlMode::position);
  kinematics.set_joint_control_mode(hardware::Joint::left_ankle, hardware::JointControlMode::position);
  kinematics.set_joint_control_mode(hardware::Joint::right_ankle, hardware::JointControlMode::position);
  kinematics.set_joint_control_mode(hardware::Joint::right_wheel, hardware::JointControlMode::position);
  kinematics.set_joint_position({
    { hardware::Joint::left_ankle, initial_ankle_angle - ANGLE },
    { hardware::Joint::right_ankle, initial_ankle_angle },
  });
  roll_controller.reset();
}

void LocomotionUnicycle::on_stop() {
  kinematics.set_joint_control_mode(hardware::Joint::left_wheel, hardware::JointControlMode::off);
  kinematics.set_joint_control_mode(hardware::Joint::left_ankle, hardware::JointControlMode::off);
  kinematics.set_joint_control_mode(hardware::Joint::right_ankle, hardware::JointControlMode::off);
  kinematics.set_joint_control_mode(hardware::Joint::right_wheel, hardware::JointControlMode::off);
}

void LocomotionUnicycle::up(bool key_down, std::set<std::string>& modifiers) {
  // TODO: implement
}

void LocomotionUnicycle::down(bool key_down, std::set<std::string>& modifiers) {
  // TODO: implement
}
