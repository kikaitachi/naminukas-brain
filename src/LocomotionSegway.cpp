#include "LocomotionSegway.hpp"

LocomotionSegway::LocomotionSegway(hardware::Kinematics& kinematics, IMU& imu)
    : Locomotion(100), kinematics(kinematics), imu(imu) {
}

std::string LocomotionSegway::name() {
  return "Segway";
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

void LocomotionSegway::control_loop() {
  std::vector<hardware::JointPosition> curr_pos = kinematics.get_joint_position({
    hardware::Joint::left_wheel,
    hardware::Joint::right_wheel
  });
  float rpm_left = (curr_pos[0].degrees - prev_pos[0].degrees) * 60.0 * 1000000000 / 360 / control_loop_nanos;
  float rpm_right = (-curr_pos[1].degrees + prev_pos[1].degrees) * 60.0 * 1000000000 / 360 / control_loop_nanos;
  float rpm_avg = (rpm_left + rpm_right) / 2;
  float new_rpm = RPM_ALPHA * rpm_avg + (1 - RPM_ALPHA) * prev_rpm;
  prev_rpm = new_rpm;
  float goal_pitch = (new_rpm - goal_rpm) * 0.2;
  float pitch = imu.get_pitch();
  float error = pitch - goal_pitch;
  float p = 2.5;
  float input = error * p;
  kinematics.set_joint_position({
    { hardware::Joint::left_wheel, curr_pos[0].degrees - input },
    { hardware::Joint::right_wheel, curr_pos[1].degrees + input }
  });
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
  prev_rpm = 0;
  goal_rpm = 0;
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
  if (key_down) {
    goal_rpm = 18;
  } else {
    goal_rpm = 0;
  }
}

void LocomotionSegway::down(bool key_down) {
  if (key_down) {
    goal_rpm = -18;
  } else {
    goal_rpm = 0;
  }
}
