#include "DynamixelKinematics.hpp"
#include "Logger.hpp"

static int joint2id(hardware::Joint joint) {
  switch (joint) {
    case hardware::Joint::left_wheel:
      return 2;
    case hardware::Joint::left_ankle:
      return 3;
    case hardware::Joint::right_ankle:
      return 4;
    case hardware::Joint::right_wheel:
      return 5;
    default:
      logger::error("Can't provide servo id for unknown joint type: %d", (int)joint);
      return -1;
  }
}

DynamixelKinematics::DynamixelKinematics() {
  portHandler = dynamixel::PortHandler::getPortHandler("/dev/ttyUSB0");
  if (portHandler->openPort()) {
    logger::info("Succeeded to open Dynamixel port");
  }
  else {
    logger::error("Failed to open Dynamixel port");
  }
}

DynamixelKinematics::~DynamixelKinematics() {
  portHandler->closePort();
}

void DynamixelKinematics::set_joint_control_mode(
    hardware::Joint joint, hardware::JointControlMode mode, double max_acceleration, double max_rpm, double millis) {
  // TODO: implement
}

void DynamixelKinematics::set_joint_position(hardware::Joint joint, double degrees) {
  // TODO: implement
}

void DynamixelKinematics::set_joint_speed(hardware::Joint joint, double rpm) {
  // TODO: implement
}

double DynamixelKinematics::get_joint_position(hardware::Joint joint) {
  // TODO: implement
  return 0;
}
