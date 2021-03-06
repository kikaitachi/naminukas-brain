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
      logger::error("Can't provide servo id for unknown joint type: %d", static_cast<int>(joint));
      return -1;
  }
}

DynamixelKinematics::DynamixelKinematics() {
  dynamixel_connection = new DynamixelConnection(DEVICENAME, PROTOCOL_VERSION, BAUDRATE);
  map_indirect(0, dynamixel_XM430W350.present_current());
  map_indirect(4, dynamixel_XM430W350.present_velocity());
  map_indirect(12, dynamixel_XM430W350.present_position());
}

DynamixelKinematics::~DynamixelKinematics() {
  delete dynamixel_connection;
}

void DynamixelKinematics::set_joint_control_mode(
    hardware::Joint joint, hardware::JointControlMode mode,
    double max_acceleration, double max_rpm, int acceleration_ms, int total_ms) {
  int id = joint2id(joint);
  DynamixelModel& model = joint2model(joint);
  switch (mode) {
    case hardware::JointControlMode::off:
      dynamixel_connection->write(model.torque_enable(), { { id, 0 } });
      break;
    case hardware::JointControlMode::position:
      dynamixel_connection->write(model.torque_enable(), { { id, 0 } });
      dynamixel_connection->write(model.drive_mode(), { { id, DYNAMIXEL_DRIVE_MODE_VELOCITY_BASED } });
      dynamixel_connection->write(model.operating_mode(), { { id, DYNAMIXEL_OPERATING_MODE_EXTENDED_POSITION } });
      dynamixel_connection->write(model.profile_acceleration(), { { id, model.rpm2_to_value(max_acceleration) } });
      dynamixel_connection->write(model.profile_velocity(), { { id, model.rpm_to_value(max_rpm) } });
      dynamixel_connection->write(model.torque_enable(), { { id, 1 } });
      break;
    case hardware::JointControlMode::velocity:
      dynamixel_connection->write(model.torque_enable(), { { id, 0 } });
      dynamixel_connection->write(model.drive_mode(), { { id, DYNAMIXEL_DRIVE_MODE_VELOCITY_BASED } });
      dynamixel_connection->write(model.operating_mode(), { { id, DYNAMIXEL_OPERATING_MODE_VELOCITY } });
      dynamixel_connection->write(model.profile_acceleration(), { { id, model.rpm2_to_value(max_acceleration) } });
      dynamixel_connection->write(model.profile_velocity(), { { id, model.rpm_to_value(max_rpm) } });
      dynamixel_connection->write(model.torque_enable(), { { id, 1 } });
      break;
    case hardware::JointControlMode::time:
      dynamixel_connection->write(model.torque_enable(), { { id, 0 } });
      dynamixel_connection->write(model.drive_mode(), { { id, DYNAMIXEL_DRIVE_MODE_TIME_BASED } });
      dynamixel_connection->write(model.operating_mode(), { { id, DYNAMIXEL_OPERATING_MODE_EXTENDED_POSITION } });
      dynamixel_connection->write(model.profile_acceleration(), { { id, acceleration_ms } });
      dynamixel_connection->write(model.profile_velocity(), { { id, total_ms } });
      dynamixel_connection->write(model.torque_enable(), { { id, 1 } });
      break;
    case hardware::JointControlMode::pwm:
      dynamixel_connection->write(model.torque_enable(), { { id, 0 } });
      dynamixel_connection->write(model.drive_mode(), { { id, DYNAMIXEL_DRIVE_MODE_VELOCITY_BASED } });
      dynamixel_connection->write(model.operating_mode(), { { id, DYNAMIXEL_OPERATING_MODE_PWM } });
      dynamixel_connection->write(model.torque_enable(), { { id, 1 } });
      break;
    default:
      logger::error("Can't set joint %d control to unsupported mode %d",
        static_cast<int>(joint), static_cast<int>(mode));
      break;
  }
}

void DynamixelKinematics::set_joint_position(std::vector<hardware::JointPosition> positions) {
  if (positions.empty()) {
    return;
  }
  std::vector<DynamixelControlValue> values;
  for (auto& position : positions) {
    values.push_back({
      joint2id(position.joint),
      static_cast<int>(round(position.degrees * joint2model(position.joint).positions_per_rotation() / 360.0))
    });
  }
  dynamixel_connection->write(dynamixel_XM430W350.goal_position(), values);
}

void DynamixelKinematics::set_joint_speed(std::vector<hardware::JointSpeed> speeds) {
  if (speeds.empty()) {
    return;
  }
  std::vector<DynamixelControlValue> values;
  for (auto& speed : speeds) {
    values.push_back({ joint2id(speed.joint), joint2model(speed.joint).rpm_to_value(speed.rpm) });
  }
  dynamixel_connection->write(dynamixel_XM430W350.goal_velocity(), values);
}

void DynamixelKinematics::set_joint_pwm(std::vector<hardware::JointPWM> pwms) {
  if (pwms.empty()) {
    return;
  }
  std::vector<DynamixelControlValue> values;
  for (auto& pwm : pwms) {
    values.push_back({ joint2id(pwm.joint), joint2model(pwm.joint).pwm_to_value(pwm.pwm) });
  }
  dynamixel_connection->write(dynamixel_XM430W350.goal_pwm(), values);
}

std::vector<hardware::JointPosition> DynamixelKinematics::get_joint_position(std::vector<hardware::Joint> joints) {
  std::vector<int> ids;
  for (auto& joint : joints) {
    ids.push_back(joint2id(joint));
  }
  std::vector<hardware::JointPosition> result;
  std::vector<int> positions = dynamixel_connection->read(dynamixel_XM430W350.present_position(), ids);
  for (int i = 0; i < positions.size(); i++) {
    result.push_back({ joints[i], positions[i] * 360.0 / joint2model(joints[i]).positions_per_rotation() });
  }
  for (auto& listener : position_listeners) {
    listener(result);
  }
  return result;
}

std::vector<hardware::JointSpeed> DynamixelKinematics::get_joint_speed(std::vector<hardware::Joint> joints) {
  std::vector<int> ids;
  for (auto& joint : joints) {
    ids.push_back(joint2id(joint));
  }
  std::vector<hardware::JointSpeed> result;
  std::vector<int> velocities = dynamixel_connection->read(dynamixel_XM430W350.present_velocity(), ids);
  for (int i = 0; i < velocities.size(); i++) {
    result.push_back({ joints[i], joint2model(joints[i]).value_to_rpm(velocities[i]) });
  }
  return result;
}

std::vector<hardware::JointState> DynamixelKinematics::get_joint_state(std::vector<hardware::Joint> joints) {
  std::vector<hardware::JointState> result;
  std::vector<int> ids;
  for (auto& joint : joints) {
    ids.push_back(joint2id(joint));
  }
  // TODO: extract magic number 224
  std::vector<std::vector<int>> data = dynamixel_connection->read(224, { 2, 4, 4 }, ids);
  for (int i = 0; i < data.size(); i++) {
    DynamixelModel& model = joint2model(joints[i]);
    result.push_back({
      data[i][2] * 360.0f / model.positions_per_rotation(),
      model.value_to_rpm(data[i][1]),
      model.value_to_current(static_cast<int16_t>(data[i][0]))
    });
  }
  return result;
}

bool DynamixelKinematics::reached_destination(std::vector<hardware::Joint> joints) {
  std::vector<int> ids;
  for (auto& joint : joints) {
    ids.push_back(joint2id(joint));
  }
  for (int moving : dynamixel_connection->read(dynamixel_XM430W350.moving(), ids)) {
    if (moving) {
      return false;
    }
  }
  return true;
}

DynamixelModel& DynamixelKinematics::joint2model(hardware::Joint joint) {
  return dynamixel_XM430W350;
}

void DynamixelKinematics::map_indirect(int offset, DynamixelControlItem item) {
  for (int i = 0; i < item.size; i++) {
    dynamixel_connection->write(dynamixel_XM430W350.indirect_address(offset + i * 2), {
      { 2, item.address + i },
      { 3, item.address + i },
      { 4, item.address + i },
      { 5, item.address + i }
    });
  }
}
