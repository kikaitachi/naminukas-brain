#include "LocomotionUnicycle.hpp"

#define STATIONARY_ANGLE 60
#define WALKING_ANGLE 6
#define GLIDE_RATIO 2.5

LocomotionUnicycle::LocomotionUnicycle(hardware::Kinematics& kinematics, IMU& imu)
    : Locomotion(100), roll_controller(2, 0, 0, 20, -3, 3), kinematics(kinematics), imu(imu) {
}

std::string LocomotionUnicycle::name() {
  return "Unicycle";
}

Pose LocomotionUnicycle::control_loop(Pose pose) {
  float roll = imu.get_roll();
  float goal_roll = -37;
  float roll_input = roll_controller.input(roll, goal_roll);

  /*kinematics.set_joint_position({
    { hardware::Joint::left_ankle, initial_ankle_angle - STATIONARY_ANGLE - roll_input },
    { hardware::Joint::left_wheel, - roll_input }
  });*/
  logger::debug("roll: %f, angle: %f, fitness: %f", roll, STATIONARY_ANGLE + roll_input, roll_controller.get_fitness());

  // TODO: calculate new pose
  return pose;
}

void LocomotionUnicycle::on_start() {
  kinematics.set_joint_control_mode(hardware::Joint::left_wheel, hardware::JointControlMode::time, 0, 0, 0, 100);
  kinematics.set_joint_control_mode(hardware::Joint::left_ankle, hardware::JointControlMode::time, 0, 0, 0, 100);
  kinematics.set_joint_control_mode(hardware::Joint::right_ankle, hardware::JointControlMode::time, 0, 0, 0, 100);
  kinematics.set_joint_control_mode(hardware::Joint::right_wheel, hardware::JointControlMode::time, 0, 0, 0, 100);
  kinematics.set_joint_position({
    { hardware::Joint::left_ankle, initial_ankle_angle - STATIONARY_ANGLE },
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
  std::vector<hardware::JointPosition> curr_pos = kinematics.get_joint_position({
    hardware::Joint::left_wheel
  });
  double angle = modifiers.find("Control") != modifiers.end() ? WALKING_ANGLE : -WALKING_ANGLE;
  if (curr_pos.size() != 0) {
    kinematics.set_joint_position({
      { hardware::Joint::left_ankle, initial_ankle_angle - STATIONARY_ANGLE + angle },
      { hardware::Joint::left_wheel, curr_pos[0].degrees + angle * GLIDE_RATIO },
    });
  }
}

void LocomotionUnicycle::down(bool key_down, std::set<std::string>& modifiers) {
  // TODO: implement
}
