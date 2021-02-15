#include <cmath>

#include "LocomotionGallop.hpp"

#define TILT_ANGLE 45
#define POS_REACHED_THRESHOLD 5

LocomotionGallop::LocomotionGallop(hardware::Kinematics& kinematics)
    : Locomotion(50), kinematics(kinematics) {
}

std::string LocomotionGallop::name() {
  return "Gallop";
}

Pose LocomotionGallop::control_loop(Pose pose) {
  std::vector<hardware::JointPosition> current_pos = kinematics.get_joint_position({
    hardware::Joint::left_ankle, hardware::Joint::right_ankle
  });
  if (current_pos.size() == 2) {
    if (going_up) {
      if ((fabs(current_pos[0].degrees - initial_ankle_angle - TILT_ANGLE) <= POS_REACHED_THRESHOLD) &&
          (fabs(current_pos[1].degrees - initial_ankle_angle - TILT_ANGLE) <= POS_REACHED_THRESHOLD)) {
        going_up = false;
        logger::info("Top reached, going down");
        kinematics.set_joint_position({
          { hardware::Joint::left_ankle, initial_ankle_angle },
          { hardware::Joint::right_ankle, initial_ankle_angle },
          { hardware::Joint::left_wheel, initial_pos[0].degrees },
          { hardware::Joint::right_wheel, initial_pos[1].degrees }
        });
      }
    } else {
      if ((fabs(current_pos[0].degrees - initial_ankle_angle) <= POS_REACHED_THRESHOLD) &&
          (fabs(current_pos[1].degrees - initial_ankle_angle) <= POS_REACHED_THRESHOLD)) {
        logger::info("Bottom reached, going up");
        going_up = true;
        kinematics.set_joint_position({
          { hardware::Joint::left_ankle, initial_ankle_angle + TILT_ANGLE },
          { hardware::Joint::right_ankle, initial_ankle_angle + TILT_ANGLE },
          { hardware::Joint::left_wheel, initial_pos[0].degrees + TILT_ANGLE },
          { hardware::Joint::right_wheel, initial_pos[1].degrees + TILT_ANGLE }
        });
      }
    }
  }
  // Return original pose. Wheel odometry might be not
  // possible to calculate in this mode of locomotion.
  return pose;
}

void LocomotionGallop::on_start() {
  kinematics.set_joint_control_mode(hardware::Joint::left_wheel, hardware::JointControlMode::position);
  kinematics.set_joint_control_mode(hardware::Joint::left_ankle, hardware::JointControlMode::position);
  kinematics.set_joint_control_mode(hardware::Joint::right_ankle, hardware::JointControlMode::position);
  kinematics.set_joint_control_mode(hardware::Joint::right_wheel, hardware::JointControlMode::position);
  kinematics.set_joint_position({
    { hardware::Joint::left_ankle, initial_ankle_angle },
    { hardware::Joint::right_ankle, initial_ankle_angle }
  });
  initial_pos = kinematics.get_joint_position({
    hardware::Joint::left_wheel, hardware::Joint::right_wheel
  });
  going_up = false;
}

void LocomotionGallop::on_stop() {
  kinematics.set_joint_control_mode(hardware::Joint::left_wheel, hardware::JointControlMode::off);
  kinematics.set_joint_control_mode(hardware::Joint::left_ankle, hardware::JointControlMode::off);
  kinematics.set_joint_control_mode(hardware::Joint::right_ankle, hardware::JointControlMode::off);
  kinematics.set_joint_control_mode(hardware::Joint::right_wheel, hardware::JointControlMode::off);
}
