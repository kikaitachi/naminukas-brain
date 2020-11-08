#include "DynamixelKinematics.hpp"
#include "Logger.hpp"

#define DEVICENAME "/dev/ttyUSB0"
#define PROTOCOL_VERSION 2.0
#define BAUDRATE 4000000

#define POSITIONS_PER_RPM 4096

#define DRIVE_MODE 10
#define OPERATING_MODE 11
#define TORQUE_ENABLE 64
#define GOAL_VELOCITY 104
#define PROFILE_ACCELERATION 108
#define PROFILE_VELOCITY 112
#define GOAL_POSITION 116
#define MOVING 122
#define MOVING_STATUS 123

#define DRIVE_MODE_VELOCITY_BASED 0
#define DRIVE_MODE_TIME_BASED 4

#define OPERATING_MODE_CURRENT 0
#define OPERATING_MODE_VELOCITY 1
#define OPERATING_MODE_POSITION 3
#define OPERATING_MODE_EXTENDED_POSITION 4
#define OPERATING_MODE_CURRENT_BASED_POSITION 5
#define OPERATING_MODE_PWM 16

#define ACCELERATION 214.577 // rev/min^2
#define VELOCITY 0.229 // rev/min

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
      write_byte(id, DRIVE_MODE, DRIVE_MODE_VELOCITY_BASED);
      write_byte(id, OPERATING_MODE, OPERATING_MODE_POSITION);
      enable_torque(id, 1);
      break;
    case hardware::JointControlMode::velocity:
      enable_torque(id, 0);
      write_byte(id, DRIVE_MODE, DRIVE_MODE_VELOCITY_BASED);
      write_byte(id, OPERATING_MODE, OPERATING_MODE_VELOCITY);
      enable_torque(id, 1);
      break;
    case hardware::JointControlMode::time:
      enable_torque(id, 0);
      write_byte(id, DRIVE_MODE, DRIVE_MODE_TIME_BASED);
      write_byte(id, OPERATING_MODE, OPERATING_MODE_POSITION);
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

bool DynamixelKinematics::write_byte(int id, int address, int value) {
  uint8_t dxl_error = 0;
  int dxl_comm_result = packet_handler->write1ByteTxRx(port_handler, id, address, value, &dxl_error);
  if (dxl_comm_result != COMM_SUCCESS) {
    logger::error("Failed to write %d to addres %d for Dynamixel with id %d: %s",
      value, address, id, packet_handler->getTxRxResult(dxl_comm_result));
      return false;
  } else if (dxl_error != 0) {
    logger::error("Failed to write %d to address %d for Dynamixel with id %d: %s",
    value, address, id, packet_handler->getRxPacketError(dxl_error));
    return false;
  }
  return true;
}

bool DynamixelKinematics::enable_torque(int id, bool enable) {
  return write_byte(id, TORQUE_ENABLE, enable ? 1 : 0);
}
