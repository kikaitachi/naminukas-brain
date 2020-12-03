#include "LocomotionPole.hpp"
#include "Logger.hpp"

LocomotionPole::LocomotionPole(hardware::Kinematics& kinematics, Model& model)
    : kinematics(kinematics), model(model) {
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
      std::vector<double> previous_angles = kinematics.get_joint_position({
        hardware::Joint::left_wheel,
        hardware::Joint::right_wheel,
        hardware::Joint::left_ankle,
        hardware::Joint::right_ankle
      });
      while (!stopped) {
        std::vector<double> current_angles = kinematics.get_joint_position({
          hardware::Joint::left_wheel,
          hardware::Joint::right_wheel,
          hardware::Joint::left_ankle,
          hardware::Joint::right_ankle
        });
        if (previous_angles != current_angles) {
          logger::debug("Moving");
          double angular_dist = current_angles[0] - previous_angles[0];
          double delta_x = 0; // TODO: sin(heading) * (angular_dist * degrees_to_dist)
          double delta_y = 0;
          model.move(delta_x, 0, delta_y);
          // TODO: implement rotation and moving of ankles
          current_angles = previous_angles;
        } else {
          logger::debug("Stationary");
        }
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
