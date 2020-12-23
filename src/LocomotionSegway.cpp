#include "LocomotionSegway.hpp"
#include "Logger.hpp"

LocomotionSegway::LocomotionSegway(hardware::Kinematics& kinematics, IMU& imu)
    : Locomotion(100), kinematics(kinematics), imu(imu) {
  //
}

std::string LocomotionSegway::name() {
  return "Segway";
}

float clamp(float value, float min, float max) {
  if (value < min) {
    return min;
  }
  if (value > max) {
    return max;
  }
  return value;
}

void LocomotionSegway::control_loop() {
  std::vector<hardware::JointPosition> curr_pos = kinematics.get_joint_position({
    hardware::Joint::left_wheel,
    hardware::Joint::right_wheel
  });
  float rpm_left = (curr_pos[0].degrees - prev_pos[0].degrees) * 60.0 * 1000000000 / 360 / control_loop_nanos;
  float rpm_right = (curr_pos[1].degrees - prev_pos[1].degrees) * 60.0 * 1000000000 / 360 / control_loop_nanos;
  float rpm_avg = (rpm_left + rpm_right) / 2;
  // TODO: control expected_pitch to limit max speed (20 RPM?)
  float goal_rpm = 0;
  float goal_pitch = (rpm_avg - goal_rpm) * 0.1;
  float pitch = imu.get_pitch();
  float error = pitch - goal_pitch;//expected_pitch;
  float p = 2.5;
  float d = 0;
  float input = error * p + (error - prev_error) * d;
  logger::debug("rpm left: %f, right: %f, avg: %f", rpm_left, rpm_right, rpm_avg);
  float left_wheel_bias = clamp(expected_pos[0].degrees - curr_pos[0].degrees, -max_bias, max_bias);
  float right_wheel_bias = clamp(expected_pos[1].degrees - curr_pos[1].degrees, -max_bias, max_bias);
  kinematics.set_joint_position({
    { hardware::Joint::left_wheel, curr_pos[0].degrees - input + left_wheel_bias },
    { hardware::Joint::right_wheel, curr_pos[1].degrees + input + right_wheel_bias }
  });
  prev_error = error;
  prev_pos = curr_pos;
}

void LocomotionSegway::on_start() {
  kinematics.set_joint_control_mode(hardware::Joint::left_wheel, hardware::JointControlMode::position);
  kinematics.set_joint_control_mode(hardware::Joint::left_ankle, hardware::JointControlMode::position, 100);
  kinematics.set_joint_control_mode(hardware::Joint::right_ankle, hardware::JointControlMode::position, 100);
  kinematics.set_joint_control_mode(hardware::Joint::right_wheel, hardware::JointControlMode::position);
  kinematics.set_joint_position({
    { hardware::Joint::left_ankle, initial_ankle_angle - 90 },
    { hardware::Joint::right_ankle, initial_ankle_angle + 90 },
  });
  prev_error = 0;
  expected_pos = kinematics.get_joint_position({
    hardware::Joint::left_wheel,
    hardware::Joint::right_wheel
  });
  prev_pos = expected_pos;
}

void LocomotionSegway::on_stop() {
  kinematics.set_joint_control_mode(hardware::Joint::left_wheel, hardware::JointControlMode::off);
  kinematics.set_joint_control_mode(hardware::Joint::left_ankle, hardware::JointControlMode::off);
  kinematics.set_joint_control_mode(hardware::Joint::right_ankle, hardware::JointControlMode::off);
  kinematics.set_joint_control_mode(hardware::Joint::right_wheel, hardware::JointControlMode::off);
}

void LocomotionSegway::up(bool key_down) {
  // TODO: implement
}

void LocomotionSegway::down(bool key_down) {
  // TODO: implement
}
