#include "LocomotionPole.hpp"
#include "Logger.hpp"

LocomotionPole::LocomotionPole(hardware::Kinematics& kinematics) : kinematics(kinematics) {
}

std::string LocomotionPole::name() {
  return "Pole";
}

void LocomotionPole::start() {
  if (stopped) {
    kinematics.set_joint_control_mode(hardware::Joint::left_wheel, hardware::JointControlMode::velocity);
    kinematics.set_joint_control_mode(hardware::Joint::left_ankle, hardware::JointControlMode::position);
    kinematics.set_joint_control_mode(hardware::Joint::right_ankle, hardware::JointControlMode::position);
    kinematics.set_joint_control_mode(hardware::Joint::right_wheel, hardware::JointControlMode::velocity);
    kinematics.set_joint_position({
      { hardware::Joint::left_ankle, initial_ankle_angle + 10 },
      { hardware::Joint::right_ankle, initial_ankle_angle - 10 },
    });
    odometry_thread = new std::thread([&]() {
      stopped = false;
      while (!stopped) {
        // TODO: calculate odometry
        kinematics.get_joint_position({
          hardware::Joint::left_wheel,
          hardware::Joint::right_wheel,
          hardware::Joint::left_ankle,
          hardware::Joint::right_ankle
        });
        logger::info("Test");
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
      }
    });
  }
}

void LocomotionPole::stop() {
  if (!stopped) {
    kinematics.set_joint_control_mode(hardware::Joint::left_wheel, hardware::JointControlMode::off);
    kinematics.set_joint_control_mode(hardware::Joint::left_ankle, hardware::JointControlMode::off);
    kinematics.set_joint_control_mode(hardware::Joint::right_ankle, hardware::JointControlMode::off);
    kinematics.set_joint_control_mode(hardware::Joint::right_wheel, hardware::JointControlMode::off);
    stopped = true;
    odometry_thread->join();
    delete odometry_thread;
  }
}

void LocomotionPole::halt() {
  kinematics.set_joint_speed({ { hardware::Joint::left_wheel, 0 }, { hardware::Joint::right_wheel, 0 } });
}

void LocomotionPole::up(bool key_down) {
  if (key_down) {
    kinematics.set_joint_speed({ { hardware::Joint::left_wheel, -max_rpm }, { hardware::Joint::right_wheel, max_rpm } });
  } else {
    halt();
  }
}

void LocomotionPole::down(bool key_down) {
  if (key_down) {
    kinematics.set_joint_speed({ { hardware::Joint::left_wheel, max_rpm }, { hardware::Joint::right_wheel, -max_rpm } });
  } else {
    halt();
  }
}

void LocomotionPole::left(bool key_down) {
  if (key_down) {
    kinematics.set_joint_speed({ { hardware::Joint::left_wheel, max_rpm }, { hardware::Joint::right_wheel, max_rpm } });
  } else {
    halt();
  }
}

void LocomotionPole::right(bool key_down) {
  if (key_down) {
    kinematics.set_joint_speed({ { hardware::Joint::left_wheel, -max_rpm }, { hardware::Joint::right_wheel, -max_rpm } });
  } else {
    halt();
  }
}
