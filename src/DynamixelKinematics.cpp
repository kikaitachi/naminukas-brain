#include <cmath>

#include "DynamixelKinematics.hpp"
#include "Logger.hpp"

#define DEVICENAME "/dev/ttyUSB0"
#define PROTOCOL_VERSION 2.0
#define BAUDRATE 4000000

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
  dynamixel_connection = new DynamixelConnection(DEVICENAME, PROTOCOL_VERSION, BAUDRATE);
}

DynamixelKinematics::~DynamixelKinematics() {
  delete dynamixel_connection;
}

void DynamixelKinematics::set_joint_control_mode(
    hardware::Joint joint, hardware::JointControlMode mode, double max_acceleration, double max_rpm, double millis) {
  int id = joint2id(joint);
  switch (mode) {
    case hardware::JointControlMode::off:
      dynamixel_connection->write(dynamixel_XM430W350.torque_enable(), { { id, 0 } });
      break;
    case hardware::JointControlMode::position:
      dynamixel_connection->write(dynamixel_XM430W350.torque_enable(), { { id, 0 } });
      dynamixel_connection->write(dynamixel_XM430W350.drive_mode(), { { id, DYNAMIXEL_DRIVE_MODE_VELOCITY_BASED } });
      dynamixel_connection->write(dynamixel_XM430W350.operating_mode(), { { id, DYNAMIXEL_OPERATING_MODE_POSITION } });
      dynamixel_connection->write(dynamixel_XM430W350.torque_enable(), { { id, 1 } });
      break;
    case hardware::JointControlMode::velocity:
      dynamixel_connection->write(dynamixel_XM430W350.torque_enable(), { { id, 0 } });

      dynamixel_connection->write(dynamixel_XM430W350.drive_mode(), { { id, DYNAMIXEL_DRIVE_MODE_VELOCITY_BASED } });
      dynamixel_connection->write(dynamixel_XM430W350.operating_mode(), { { id, DYNAMIXEL_OPERATING_MODE_VELOCITY } });
      dynamixel_connection->write(dynamixel_XM430W350.torque_enable(), { { id, 1 } });
      break;
    case hardware::JointControlMode::time:
      dynamixel_connection->write(dynamixel_XM430W350.torque_enable(), { { id, 0 } });
      dynamixel_connection->write(dynamixel_XM430W350.drive_mode(), { { id, DYNAMIXEL_DRIVE_MODE_TIME_BASED } });
      dynamixel_connection->write(dynamixel_XM430W350.operating_mode(), { { id, DYNAMIXEL_OPERATING_MODE_POSITION } });
      dynamixel_connection->write(dynamixel_XM430W350.torque_enable(), { { id, 1 } });
      break;
    default:
    logger::error("Can't set joint %d control to unsupported mode %d", (int)joint, (int)mode);
      break;
  }
}

void DynamixelKinematics::set_joint_position(std::vector<hardware::JointPosition> positions) {
  std::vector<DynamixelControlValue> values;
  for (auto& position : positions) {
    values.push_back({ joint2id(position.joint), (int)round(position.degrees * dynamixel_XM430W350.positions_per_rotation() / 360.0) });
  }
  dynamixel_connection->write(dynamixel_XM430W350.goal_position(), values);
}

void DynamixelKinematics::set_joint_speed(std::vector<hardware::JointSpeed> speeds) {
  std::vector<DynamixelControlValue> values;
  for (auto& speed : speeds) {
    values.push_back({ joint2id(speed.joint), dynamixel_XM430W350.rpm_to_value(speed.rpm) });
  }
  dynamixel_connection->write(dynamixel_XM430W350.goal_velocity(), values);
}

double DynamixelKinematics::get_joint_position(hardware::Joint joint) {
  // TODO: implement
  return 0;
}
