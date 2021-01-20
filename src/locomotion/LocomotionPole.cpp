#include "LocomotionPole.hpp"

LocomotionPole::LocomotionPole(hardware::Kinematics& kinematics, Model& model)
    : Locomotion(10), kinematics(kinematics), model(model) {
}

std::string LocomotionPole::name() {
  return "Pole";
}

void LocomotionPole::control_loop() {
  std::vector<hardware::JointPosition> current_angles = kinematics.get_joint_position({
    hardware::Joint::left_wheel,
    hardware::Joint::right_wheel,
    hardware::Joint::left_ankle,
    hardware::Joint::right_ankle
  });
  /*if (previous_angles != current_angles) {
    logger::debug("Moving");
    double angular_dist = current_angles[0].degrees - previous_angles[0].degrees;
    double delta_x = 0; // TODO: sin(heading) * (angular_dist * degrees_to_dist)
    double delta_y = 0;
    //model.move(delta_x, 0, delta_y);
    // TODO: implement rotation and moving of ankles
    current_angles = previous_angles;
  } else {
    logger::debug("Stationary");
  }*/
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
  // TODO: init odometry
  std::vector<hardware::JointPosition> previous_angles = kinematics.get_joint_position({
    hardware::Joint::left_wheel,
    hardware::Joint::right_wheel,
    hardware::Joint::left_ankle,
    hardware::Joint::right_ankle
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
    kinematics.set_joint_speed({ { hardware::Joint::left_wheel, -max_rpm }, { hardware::Joint::right_wheel, max_rpm } });
  } else {
    halt();
  }
}

void LocomotionPole::down(bool key_down, std::set<std::string>& modifiers) {
  if (key_down) {
    kinematics.set_joint_speed({ { hardware::Joint::left_wheel, max_rpm }, { hardware::Joint::right_wheel, -max_rpm } });
  } else {
    halt();
  }
}

void LocomotionPole::left(bool key_down, std::set<std::string>& modifiers) {
  if (key_down) {
    kinematics.set_joint_speed({ { hardware::Joint::left_wheel, max_rpm }, { hardware::Joint::right_wheel, max_rpm } });
  } else {
    halt();
  }
}

void LocomotionPole::right(bool key_down, std::set<std::string>& modifiers) {
  if (key_down) {
    kinematics.set_joint_speed({ { hardware::Joint::left_wheel, -max_rpm }, { hardware::Joint::right_wheel, -max_rpm } });
  } else {
    halt();
  }
}