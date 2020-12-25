#include "LocomotionSki.hpp"
#include "Logger.hpp"

LocomotionSki::LocomotionSki(hardware::Kinematics& kinematics, IMU& imu)
    : Locomotion(100), kinematics(kinematics), imu(imu) {
}

std::string LocomotionSki::name() {
  return "Ski";
}

void LocomotionSki::control_loop() {
  float pitch = imu.get_pitch();
  float error = pitch - expected_pitch;
  float p = 2.5;
  float d = 0.1;
  float input = error * p + (error - prev_error) * d;
  //logger::debug("pitch: %f, error: %f, input: %f", pitch, error, input);
  if (input < -max_ankle_change) {
      input = -max_ankle_change;
  }
  if (input > max_ankle_change) {
      input = max_ankle_change;
  }
  kinematics.set_joint_position({
    { hardware::Joint::left_ankle, initial_ankle_angle + input },
    { hardware::Joint::right_ankle, initial_ankle_angle + input }
  });
  prev_error = error;
}

void LocomotionSki::on_start() {
  if (stopped) {
    kinematics.set_joint_control_mode(hardware::Joint::left_wheel, hardware::JointControlMode::velocity);
    kinematics.set_joint_control_mode(hardware::Joint::left_ankle, hardware::JointControlMode::position);
    kinematics.set_joint_control_mode(hardware::Joint::right_ankle, hardware::JointControlMode::position);
    kinematics.set_joint_control_mode(hardware::Joint::right_wheel, hardware::JointControlMode::velocity);
    kinematics.set_joint_position({
      { hardware::Joint::left_ankle, initial_ankle_angle },
      { hardware::Joint::right_ankle, initial_ankle_angle },
    });
    kinematics.set_joint_speed({
      { hardware::Joint::left_wheel, max_rpm },
      { hardware::Joint::right_wheel, max_rpm }
    });
    prev_error = 0;
  }
}

void LocomotionSki::on_stop() {
  kinematics.set_joint_control_mode(hardware::Joint::left_wheel, hardware::JointControlMode::off);
  kinematics.set_joint_control_mode(hardware::Joint::left_ankle, hardware::JointControlMode::off);
  kinematics.set_joint_control_mode(hardware::Joint::right_ankle, hardware::JointControlMode::off);
  kinematics.set_joint_control_mode(hardware::Joint::right_wheel, hardware::JointControlMode::off);
}

void LocomotionSki::up(bool key_down, std::set<std::string> modifiers) {
  if (key_down) {
    kinematics.set_joint_speed({ { hardware::Joint::left_wheel, max_rpm }, { hardware::Joint::right_wheel, max_rpm } });
  } else {
    kinematics.set_joint_speed({ { hardware::Joint::left_wheel, 0 }, { hardware::Joint::right_wheel, 0 } });
  }
}

void LocomotionSki::down(bool key_down, std::set<std::string> modifiers) {
  if (key_down) {
    kinematics.set_joint_speed({ { hardware::Joint::left_wheel, -max_rpm }, { hardware::Joint::right_wheel, -max_rpm } });
  } else {
    kinematics.set_joint_speed({ { hardware::Joint::left_wheel, 0 }, { hardware::Joint::right_wheel, 0 } });
  }
}

void LocomotionSki::left(bool key_down, std::set<std::string> modifiers) {
  if (key_down) {
    kinematics.set_joint_speed({ { hardware::Joint::left_wheel, max_rpm + turn_rpm }, { hardware::Joint::right_wheel, max_rpm - turn_rpm } });
  } else {
    kinematics.set_joint_speed({ { hardware::Joint::left_wheel, max_rpm }, { hardware::Joint::right_wheel, max_rpm } });
  }
}

void LocomotionSki::right(bool key_down, std::set<std::string> modifiers) {
  if (key_down) {
    kinematics.set_joint_speed({ { hardware::Joint::left_wheel, max_rpm - turn_rpm }, { hardware::Joint::right_wheel, max_rpm + turn_rpm } });
  } else {
    kinematics.set_joint_speed({ { hardware::Joint::left_wheel, max_rpm }, { hardware::Joint::right_wheel, max_rpm } });
  }
}
