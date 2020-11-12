#include "DynamixelKinematics.hpp"
#include "Logger.hpp"

#define DEVICENAME "/dev/ttyUSB0"
#define PROTOCOL_VERSION 2.0
#define BAUDRATE 4000000

#define POSITIONS_PER_RPM 4096

/*
#define GOAL_VELOCITY 104
#define PROFILE_ACCELERATION 108
#define PROFILE_VELOCITY 112
#define GOAL_POSITION 116
#define MOVING 122
#define MOVING_STATUS 123
*/

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
  dynamixel_connection = new DynamixelConnection(DEVICENAME, PROTOCOL_VERSION, BAUDRATE);
  dynamixel_XM430W350 = new DynamixelXM430W350();
}

DynamixelKinematics::~DynamixelKinematics() {
  delete dynamixel_XM430W350;
  delete dynamixel_connection;
}

void DynamixelKinematics::set_joint_control_mode(
    hardware::Joint joint, hardware::JointControlMode mode, double max_acceleration, double max_rpm, double millis) {
  int id = joint2id(joint);
  switch (mode) {
    case hardware::JointControlMode::off:
      dynamixel_connection->write(dynamixel_XM430W350->torque_enable(), { { id, 0 } });
      break;
    case hardware::JointControlMode::position:
      dynamixel_connection->write(dynamixel_XM430W350->torque_enable(), { { id, 0 } });
      dynamixel_connection->write(dynamixel_XM430W350->drive_mode(), { { id, DRIVE_MODE_VELOCITY_BASED } });
      dynamixel_connection->write(dynamixel_XM430W350->operating_mode(), { { id, OPERATING_MODE_POSITION } });
      dynamixel_connection->write(dynamixel_XM430W350->torque_enable(), { { id, 1 } });
      break;
    case hardware::JointControlMode::velocity:
      dynamixel_connection->write(dynamixel_XM430W350->torque_enable(), { { id, 0 } });

      dynamixel_connection->write(dynamixel_XM430W350->drive_mode(), { { id, DRIVE_MODE_VELOCITY_BASED } });
      dynamixel_connection->write(dynamixel_XM430W350->operating_mode(), { { id, OPERATING_MODE_VELOCITY } });
      dynamixel_connection->write(dynamixel_XM430W350->torque_enable(), { { id, 1 } });
      break;
    case hardware::JointControlMode::time:
      dynamixel_connection->write(dynamixel_XM430W350->torque_enable(), { { id, 0 } });
      dynamixel_connection->write(dynamixel_XM430W350->drive_mode(), { { id, DRIVE_MODE_TIME_BASED } });
      dynamixel_connection->write(dynamixel_XM430W350->operating_mode(), { { id, OPERATING_MODE_POSITION } });
      dynamixel_connection->write(dynamixel_XM430W350->torque_enable(), { { id, 1 } });
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
