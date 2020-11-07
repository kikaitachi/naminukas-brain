#include "DynamixelKinematics.hpp"
#include "Logger.hpp"

#define DEVICENAME "/dev/ttyUSB0"
#define PROTOCOL_VERSION 2.0
#define BAUDRATE 4000000

#define TORQUE_ENABLE 64

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
  port_handler = dynamixel::PortHandler::getPortHandler(DEVICENAME);
  if (port_handler->openPort()) {
    logger::info("Succeeded to open Dynamixel port");
    packet_handler = dynamixel::PacketHandler::getPacketHandler(PROTOCOL_VERSION);
    if (!port_handler->setBaudRate(BAUDRATE)) {
      logger::error("Failed to change baudrate");
    }
  }
  else {
    logger::error("Failed to open Dynamixel port");
  }
}

DynamixelKinematics::~DynamixelKinematics() {
  port_handler->closePort();
}

void DynamixelKinematics::set_joint_control_mode(
    hardware::Joint joint, hardware::JointControlMode mode, double max_acceleration, double max_rpm, double millis) {
  int id = joint2id(joint);
  switch (mode) {
    case hardware::JointControlMode::off:
      enable_torque(id, 0);
      break;
    case hardware::JointControlMode::position:
      enable_torque(id, 0);
      // TODO: change mode
      enable_torque(id, 1);
      break;
    case hardware::JointControlMode::velocity:
      enable_torque(id, 0);
      // TODO: change mode
      enable_torque(id, 1);
      break;
    case hardware::JointControlMode::time:
      enable_torque(id, 0);
      // TODO: change mode
      enable_torque(id, 1);
      break;
    default:
    logger::error("Can't set joint %d control to unsupported mode %d", (int)joint, (int)mode);
      break;
  }
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

void DynamixelKinematics::enable_torque(int id, bool enable) {
  uint8_t dxl_error = 0;
  int value = enable ? 1 : 0;
  int dxl_comm_result = packet_handler->write1ByteTxRx(port_handler, id, TORQUE_ENABLE, value, &dxl_error);
  if (dxl_comm_result != COMM_SUCCESS) {
    logger::error("Failed to change Dynamixel with id %d torque to value %d: %s",
      id, value, packet_handler->getTxRxResult(dxl_comm_result));
  } else if (dxl_error != 0) {
    logger::error("Failed to change Dynamixel with id %d torque to value %d: %s",
    id, value, packet_handler->getRxPacketError(dxl_error));
  }
}
