#include "LocomotionUnicycle.hpp"

LocomotionUnicycle::LocomotionUnicycle(hardware::Kinematics& kinematics, IMU& imu)
    : Locomotion(100), roll_controller(0.1, 0.4, 0, 20, -15, 15), kinematics(kinematics), imu(imu) {
}

std::string LocomotionUnicycle::name() {
  return "Unicycle";
}

void LocomotionUnicycle::control_loop() {
  std::vector<hardware::JointPosition> curr_pos = kinematics.get_joint_position({
    hardware::Joint::right_ankle,
    hardware::Joint::right_wheel
  });

  float roll = imu.get_roll();
  float goal_roll = 64;
  float roll_input = roll_controller.input(roll, goal_roll);

  kinematics.set_joint_position({
    { hardware::Joint::right_ankle, initial_pos[0].degrees - roll_input },
    { hardware::Joint::right_wheel, initial_pos[1].degrees - roll_input }
  });
  logger::debug("roll: %f, fitness: %f", roll, roll_controller.get_fitness());
}

void LocomotionUnicycle::on_start() {
  kinematics.set_joint_control_mode(hardware::Joint::left_wheel, hardware::JointControlMode::position);
  kinematics.set_joint_control_mode(hardware::Joint::left_ankle, hardware::JointControlMode::position);
  kinematics.set_joint_control_mode(hardware::Joint::right_ankle, hardware::JointControlMode::position);
  kinematics.set_joint_control_mode(hardware::Joint::right_wheel, hardware::JointControlMode::position);
  kinematics.set_joint_position({
    { hardware::Joint::left_ankle, initial_ankle_angle },
    { hardware::Joint::right_ankle, initial_ankle_angle },
  });
  initial_pos = kinematics.get_joint_position({
    hardware::Joint::right_ankle,
    hardware::Joint::right_wheel
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
