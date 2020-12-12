#include "LocomotionSki.hpp"
#include "Logger.hpp"

LocomotionSki::LocomotionSki(hardware::Kinematics& kinematics, IMU& imu)
    : kinematics(kinematics), imu(imu) {
}

std::string LocomotionSki::name() {
  return "Ski";
}

void LocomotionSki::start() {
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
    odometry_thread = new std::thread([&]() {
      stopped = false;
      while (!stopped) {
        float pitch = imu.get_pitch();
        float error = pitch - expected_pitch;
        float p = 2.5;
        float input = error * p;
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
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
      }
    });
  }
}

void LocomotionSki::stop() {
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
