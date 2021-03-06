#ifndef SRC_DYNAMIXELKINEMATICS_HPP_
#define SRC_DYNAMIXELKINEMATICS_HPP_

#include <vector>

#include "DynamixelUtil.hpp"
#include "Hardware.hpp"

class DynamixelKinematics: public hardware::Kinematics {
 public:
  DynamixelKinematics();
  ~DynamixelKinematics();
  void set_joint_control_mode(hardware::Joint joint, hardware::JointControlMode mode,
    double max_acceleration, double max_rpm, int acceleration_ms, int total_ms);
  void set_joint_position(std::vector<hardware::JointPosition> positions);
  void set_joint_speed(std::vector<hardware::JointSpeed> speeds);
  void set_joint_pwm(std::vector<hardware::JointPWM> pwms);
  std::vector<hardware::JointPosition> get_joint_position(std::vector<hardware::Joint> joints);
  std::vector<hardware::JointSpeed> get_joint_speed(std::vector<hardware::Joint> joints);
  std::vector<hardware::JointState> get_joint_state(std::vector<hardware::Joint> joints);
  bool reached_destination(std::vector<hardware::Joint> joints);
 private:
  DynamixelConnection* dynamixel_connection;
  DynamixelXM430W350 dynamixel_XM430W350;
  DynamixelModel& joint2model(hardware::Joint joint);
  void map_indirect(int offset, DynamixelControlItem item);
};

#endif  // SRC_DYNAMIXELKINEMATICS_HPP_
