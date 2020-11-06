#include "DynamixelKinematics.hpp"
#include "Logger.hpp"

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

void DynamixelKinematics::set_joint_control_mode(hardware::Joint joint, hardware::JointControlMode mode, double max_acceleration, double max_rpm, double millis) {
  // TODO: implement
}

void DynamixelKinematics::set_joint_position(hardware::Joint joint, double degrees) {
  // TODO: implement
}

void DynamixelKinematics::set_joint_speed(hardware::Joint joint, double rpm) {
  // TODO: implement
}
