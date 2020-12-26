#include "LocomotionUnicycle.hpp"

LocomotionUnicycle::LocomotionUnicycle(hardware::Kinematics& kinematics, IMU& imu)
    : Locomotion(100), kinematics(kinematics), imu(imu) {
}

std::string LocomotionUnicycle::name() {
  return "Unicycle";
}

static float clamp(float value, float min, float max) {
  if (value < min) {
    return min;
  }
  if (value > max) {
    return max;
  }
  return value;
}

#define RPM_ALPHA 0.1

void LocomotionUnicycle::control_loop() {
  std::vector<hardware::JointPosition> curr_pos = kinematics.get_joint_position({
    hardware::Joint::right_ankle,
    hardware::Joint::right_wheel
  });
  float rpm_right = (prev_pos[1].degrees - curr_pos[1].degrees) * 60.0 * 1000000000 / 360 / control_loop_nanos;
  float new_rpm = RPM_ALPHA * rpm_right + (1 - RPM_ALPHA) * prev_rpm;
  prev_rpm = new_rpm;
  float goal_pitch = (new_rpm - goal_rpm) * 0.2;
  float pitch = imu.get_pitch();
  float error = pitch - goal_pitch;
  float pitch_input = error * 2.5;

  float roll = imu.get_roll();
  float goal_roll = 64;
  float roll_input = (roll - goal_roll) * 2.1;

  kinematics.set_joint_position({
    { hardware::Joint::right_ankle, clamp(curr_pos[0].degrees + roll_input, initial_ankle_angle - 15, initial_ankle_angle + 15) },
    { hardware::Joint::right_wheel, curr_pos[1].degrees + pitch_input }
  });
  prev_pos = curr_pos;
  logger::debug("roll: %f, pitch: %f", roll, pitch);
}

void LocomotionUnicycle::on_start() {
  kinematics.set_joint_control_mode(hardware::Joint::left_wheel, hardware::JointControlMode::position);
  kinematics.set_joint_control_mode(hardware::Joint::left_ankle, hardware::JointControlMode::position, 100);
  kinematics.set_joint_control_mode(hardware::Joint::right_ankle, hardware::JointControlMode::position, 100);
  kinematics.set_joint_control_mode(hardware::Joint::right_wheel, hardware::JointControlMode::position);
  kinematics.set_joint_position({
    { hardware::Joint::left_ankle, initial_ankle_angle },
    { hardware::Joint::right_ankle, initial_ankle_angle },
  });
  prev_rpm = 0;
  goal_rpm = 0;
  expected_pos = kinematics.get_joint_position({
    hardware::Joint::left_wheel,
    hardware::Joint::right_wheel
  });
  prev_pos = expected_pos;
}

void LocomotionUnicycle::on_stop() {
  kinematics.set_joint_control_mode(hardware::Joint::left_wheel, hardware::JointControlMode::off);
  kinematics.set_joint_control_mode(hardware::Joint::left_ankle, hardware::JointControlMode::off);
  kinematics.set_joint_control_mode(hardware::Joint::right_ankle, hardware::JointControlMode::off);
  kinematics.set_joint_control_mode(hardware::Joint::right_wheel, hardware::JointControlMode::off);
}

void LocomotionUnicycle::up(bool key_down, std::set<std::string>& modifiers) {
  if (key_down) {
    goal_rpm = 5;
  } else {
    goal_rpm = 0;
  }
}

void LocomotionUnicycle::down(bool key_down, std::set<std::string>& modifiers) {
  if (key_down) {
    goal_rpm = -5;
  } else {
    goal_rpm = 0;
  }
}
