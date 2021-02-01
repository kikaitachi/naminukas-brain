#include "../action/ActionRotate.hpp"
#include "LocomotionWaddle.hpp"

#define ACTION_DURATION_MS 1000
#define CONTROL_LOOP_FREQENCY 100
#define TILT_ANGLE 20
#define DRIVE_ANGLE 45

LocomotionWaddle::LocomotionWaddle(hardware::Kinematics& kinematics)
    : Locomotion(CONTROL_LOOP_FREQENCY), kinematics(kinematics),
    forward({
      std::make_shared<ActionRotate>(ActionRotate(kinematics, {
        { hardware::Joint::left_ankle, RotationType::absolute, initial_ankle_angle + TILT_ANGLE },
        { hardware::Joint::left_wheel, RotationType::relative, -DRIVE_ANGLE + TILT_ANGLE },
        { hardware::Joint::right_ankle, RotationType::absolute, initial_ankle_angle + TILT_ANGLE },
        { hardware::Joint::right_wheel, RotationType::relative, -DRIVE_ANGLE - TILT_ANGLE },
      })),
      std::make_shared<ActionRotate>(ActionRotate(kinematics, {
        { hardware::Joint::left_ankle, RotationType::absolute, initial_ankle_angle },
        { hardware::Joint::left_wheel, RotationType::relative, -DRIVE_ANGLE - TILT_ANGLE },
        { hardware::Joint::right_ankle, RotationType::absolute, initial_ankle_angle },
        { hardware::Joint::right_wheel, RotationType::relative, -DRIVE_ANGLE - TILT_ANGLE },
      })),
      std::make_shared<ActionRotate>(ActionRotate(kinematics, {
        { hardware::Joint::left_ankle, RotationType::absolute, initial_ankle_angle - TILT_ANGLE },
        { hardware::Joint::left_wheel, RotationType::relative, DRIVE_ANGLE - TILT_ANGLE },
        { hardware::Joint::right_ankle, RotationType::absolute, initial_ankle_angle - TILT_ANGLE },
        { hardware::Joint::right_wheel, RotationType::relative, DRIVE_ANGLE + TILT_ANGLE },
      })),
      std::make_shared<ActionRotate>(ActionRotate(kinematics, {
        { hardware::Joint::left_ankle, RotationType::absolute, initial_ankle_angle },
        { hardware::Joint::left_wheel, RotationType::relative, DRIVE_ANGLE + TILT_ANGLE },
        { hardware::Joint::right_ankle, RotationType::absolute, initial_ankle_angle },
        { hardware::Joint::right_wheel, RotationType::relative, DRIVE_ANGLE + TILT_ANGLE },
      }))
    }, ActionSequential::LOOP_FOREVER) {
}

std::string LocomotionWaddle::name() {
  return "Waddle";
}

void LocomotionWaddle::control_loop() {
  //forward.execute();
  int cycle = 100;
  if (control_loop_iteration % cycle == 0) {
    kinematics.set_joint_position({
      { hardware::Joint::left_ankle, initial_ankle_angle + tilt_direction * TILT_ANGLE },
      { hardware::Joint::right_ankle, initial_ankle_angle + tilt_direction * TILT_ANGLE }
    });
    tilt_direction = -tilt_direction;
  } else if ((control_loop_iteration + cycle / 2)  % cycle == 0) {
    initial_pos[0].degrees -= drive_direction * DRIVE_ANGLE;
    initial_pos[1].degrees -= drive_direction * DRIVE_ANGLE;
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
  //forward.start();
  control_loop_iteration = 0;
  tilt_direction = drive_direction = 1;
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
  //
}

void LocomotionWaddle::down(bool key_down, std::set<std::string>& modifiers) {
  if (key_down) {
    drive_direction = -drive_direction;
  }
}

void LocomotionWaddle::left(bool key_down, std::set<std::string>& modifiers) {
  //
}

void LocomotionWaddle::right(bool key_down, std::set<std::string>& modifiers) {
  //
}
