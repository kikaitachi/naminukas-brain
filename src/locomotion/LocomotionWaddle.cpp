#include "LocomotionWaddle.hpp"

#define ACTION_DURATION_MS 1000
#define CONTROL_LOOP_FREQENCY 100
#define TILT_ANGLE 20
#define DRIVE_ANGLE 50
#define INCREMENT_ANGLE 10

LocomotionWaddle::LocomotionWaddle(hardware::Kinematics& kinematics)
    : Locomotion(CONTROL_LOOP_FREQENCY), kinematics(kinematics) {
}

std::string LocomotionWaddle::name() {
  return "Waddle";
}

void LocomotionWaddle::control_loop() {
  int cycle = 100;
  if (control_loop_iteration % cycle == 0) {
    kinematics.set_joint_position({
      { hardware::Joint::left_ankle, initial_ankle_angle + tilt_direction * TILT_ANGLE },
      { hardware::Joint::right_ankle, initial_ankle_angle + tilt_direction * TILT_ANGLE }
    });
    tilt_direction = -tilt_direction;
  } else if ((control_loop_iteration + cycle / 2)  % cycle == 0) {
    initial_pos[0].degrees -= left_drive_distance * drive_direction;
    initial_pos[1].degrees -= right_drive_distance * drive_direction;
    kinematics.set_joint_position(initial_pos);
    drive_direction = -drive_direction;
  }
  control_loop_iteration++;
}

void LocomotionWaddle::on_start() {
  kinematics.set_joint_control_mode(hardware::Joint::left_wheel, hardware::JointControlMode::time, 0, 0, ACTION_DURATION_MS / 2, ACTION_DURATION_MS);
  kinematics.set_joint_control_mode(hardware::Joint::left_ankle, hardware::JointControlMode::time, 0, 0, ACTION_DURATION_MS / 2, ACTION_DURATION_MS);
  kinematics.set_joint_control_mode(hardware::Joint::right_ankle, hardware::JointControlMode::time, 0, 0, ACTION_DURATION_MS / 2, ACTION_DURATION_MS);
  kinematics.set_joint_control_mode(hardware::Joint::right_wheel, hardware::JointControlMode::time, 0, 0, ACTION_DURATION_MS / 2, ACTION_DURATION_MS);
  kinematics.set_joint_position({
    { hardware::Joint::left_ankle, initial_ankle_angle },
    { hardware::Joint::right_ankle, initial_ankle_angle }
  });
  control_loop_iteration = 0;
  tilt_direction = drive_direction = 1;
  left_drive_distance = right_drive_distance = DRIVE_ANGLE;
  initial_pos = kinematics.get_joint_position({
    hardware::Joint::left_wheel, hardware::Joint::right_wheel
  });
}

void LocomotionWaddle::on_stop() {
  kinematics.set_joint_control_mode(hardware::Joint::left_wheel, hardware::JointControlMode::off);
  kinematics.set_joint_control_mode(hardware::Joint::left_ankle, hardware::JointControlMode::off);
  kinematics.set_joint_control_mode(hardware::Joint::right_ankle, hardware::JointControlMode::off);
  kinematics.set_joint_control_mode(hardware::Joint::right_wheel, hardware::JointControlMode::off);
}

void LocomotionWaddle::up(bool key_down, std::set<std::string>& modifiers) {
  if (modifiers.find("Control") != modifiers.end()) {
    if (key_down) {
      left_drive_distance += INCREMENT_ANGLE;
      right_drive_distance += INCREMENT_ANGLE;
    }
  } else if (modifiers.find("Shift") != modifiers.end()) {
    if (key_down) {
      left_drive_distance = DRIVE_ANGLE;
      right_drive_distance = DRIVE_ANGLE;
    }
  } else {
    if (key_down) {
      left_drive_distance = DRIVE_ANGLE;
      right_drive_distance = DRIVE_ANGLE;
    } else {
      left_drive_distance = 0;
      right_drive_distance = 0;
    }
  }
}

void LocomotionWaddle::down(bool key_down, std::set<std::string>& modifiers) {
  if (modifiers.find("Control") != modifiers.end()) {
    if (key_down) {
      left_drive_distance -= INCREMENT_ANGLE;
      right_drive_distance -= INCREMENT_ANGLE;
    }
  } else if (modifiers.find("Shift") != modifiers.end()) {
    if (key_down) {
      left_drive_distance = -DRIVE_ANGLE;
      right_drive_distance = -DRIVE_ANGLE;
    }
  } else {
    if (key_down) {
      left_drive_distance = -DRIVE_ANGLE;
      right_drive_distance = -DRIVE_ANGLE;
    } else {
      left_drive_distance = 0;
      right_drive_distance = 0;
    }
  }
}

void LocomotionWaddle::left(bool key_down, std::set<std::string>& modifiers) {
  if (modifiers.find("Control") != modifiers.end()) {
    if (key_down) {
      left_drive_distance -= INCREMENT_ANGLE;
      right_drive_distance += INCREMENT_ANGLE;
    }
  } else if (modifiers.find("Shift") != modifiers.end()) {
    if (key_down) {
      left_drive_distance = -DRIVE_ANGLE;
      right_drive_distance = DRIVE_ANGLE;
    }
  } else {
    if (key_down) {
      left_drive_distance = DRIVE_ANGLE;
      right_drive_distance = DRIVE_ANGLE;
    } else {
      left_drive_distance = 0;
      right_drive_distance = 0;
    }
  }
}

void LocomotionWaddle::right(bool key_down, std::set<std::string>& modifiers) {
  if (modifiers.find("Control") != modifiers.end()) {
    if (key_down) {
      left_drive_distance += INCREMENT_ANGLE;
      right_drive_distance -= INCREMENT_ANGLE;
    }
  } else if (modifiers.find("Shift") != modifiers.end()) {
    if (key_down) {
      left_drive_distance = DRIVE_ANGLE;
      right_drive_distance = -DRIVE_ANGLE;
    }
  } else {
    if (key_down) {
      left_drive_distance = DRIVE_ANGLE;
      right_drive_distance = -DRIVE_ANGLE;
    } else {
      left_drive_distance = 0;
      right_drive_distance = 0;
    }
  }
}
