#include <cmath>
#include <limits>

#include "LocomotionSegway.hpp"

LocomotionSegway::LocomotionSegway(hardware::Kinematics& kinematics, IMU& imu)
    : Locomotion(100), kinematics(kinematics), imu(imu),
    speed_controller(0.2, 0, 0, 0, -45, 45),
    pitch_controller(2.5, 0, 0, 0, -std::numeric_limits<float>::max(), std::numeric_limits<float>::max()) {
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

  float left_diff = expected_pos[0].degrees - curr_pos[0].degrees;
  float right_diff = curr_pos[1].degrees - expected_pos[1].degrees;
  float avg_diff = (left_diff + right_diff) / 2;
  if (fabs(avg_diff) < 10) {
    avg_diff = 0;
  }
  float goal_rpm = clamp(avg_diff * 0.1, -30, 30);

  float rpm_left = (curr_pos[0].degrees - prev_pos[0].degrees) * 60.0 * 1000000000 / 360 / control_loop_nanos;
  float rpm_right = (-curr_pos[1].degrees + prev_pos[1].degrees) * 60.0 * 1000000000 / 360 / control_loop_nanos;
  float rpm_avg = (rpm_left + rpm_right) / 2;
  float new_rpm = RPM_ALPHA * rpm_avg + (1 - RPM_ALPHA) * prev_rpm;
  prev_rpm = new_rpm;
  float goal_pitch = speed_controller.input(new_rpm, goal_rpm);
  float input = pitch_controller.input(imu.get_pitch(), goal_pitch);
  kinematics.set_joint_position({
    { hardware::Joint::left_wheel, curr_pos[0].degrees - input + left_turn_speed },
    { hardware::Joint::right_wheel, curr_pos[1].degrees + input - right_turn_speed }
  });
  prev_pos = curr_pos;
  expected_pos[0].degrees += pos_speed;
  expected_pos[1].degrees -= pos_speed;

  logger::debug("current RPM: %f, diff: %f, speed fitness: %f, pitch fitness: %f",
    new_rpm, avg_diff, speed_controller.get_fitness(), pitch_controller.get_fitness());
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
  speed_controller.reset();
  pitch_controller.reset();
  expected_pos = kinematics.get_joint_position({
    hardware::Joint::left_wheel,
    hardware::Joint::right_wheel
  });
  prev_pos = expected_pos;
  pos_speed = left_turn_speed = right_turn_speed = 0;
}

void LocomotionSegway::on_stop() {
  // Move ankles to walking position
  kinematics.set_joint_position({
    { hardware::Joint::left_ankle, initial_ankle_angle },
    { hardware::Joint::right_ankle, initial_ankle_angle },
  });
  // Wait until ankles reached desired position (balancing is still in progress until this method finishes)
  for ( ; ; ) {
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    std::vector<hardware::JointPosition> position = kinematics.get_joint_position({
      hardware::Joint::left_ankle,
      hardware::Joint::right_ankle
    });
    if (abs(position[0].degrees - initial_ankle_angle) < 5 && abs(position[1].degrees - initial_ankle_angle) < 5) {
      break;
    }
  };
  // Power down motors
  kinematics.set_joint_control_mode(hardware::Joint::left_wheel, hardware::JointControlMode::off);
  kinematics.set_joint_control_mode(hardware::Joint::left_ankle, hardware::JointControlMode::off);
  kinematics.set_joint_control_mode(hardware::Joint::right_ankle, hardware::JointControlMode::off);
  kinematics.set_joint_control_mode(hardware::Joint::right_wheel, hardware::JointControlMode::off);
}

void LocomotionSegway::up(bool key_down, std::set<std::string>& modifiers) {
  if (key_down) {
    pos_speed = 2;
  } else {
    pos_speed = 0;
  }
}

void LocomotionSegway::down(bool key_down, std::set<std::string>& modifiers) {
  if (key_down) {
    pos_speed = -2;
  } else {
    pos_speed = 0;
  }
}

void LocomotionSegway::left(bool key_down, std::set<std::string>& modifiers) {
  if (key_down) {
    if (modifiers.find("Control") != modifiers.end()) {
      left_turn_speed = 0;
      right_turn_speed = -6;
    } else {
      left_turn_speed = 3;
      right_turn_speed = -3;
    }
  } else {
    left_turn_speed = right_turn_speed = 0;
  }
}

void LocomotionSegway::right(bool key_down, std::set<std::string>& modifiers) {
  if (key_down) {
    if (modifiers.find("Control") != modifiers.end()) {
      left_turn_speed = -6;
      right_turn_speed = 0;
    } else {
      left_turn_speed = -3;
      right_turn_speed = 3;
    }
  } else {
    left_turn_speed = right_turn_speed = 0;
  }
}
