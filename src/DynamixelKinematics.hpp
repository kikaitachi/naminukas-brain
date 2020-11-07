#ifndef NAMINUKAS_BRAIN_DYNAMIXEL_KINEMATICS_H_
#define NAMINUKAS_BRAIN_DYNAMIXEL_KINEMATICS_H_

#include "dynamixel_sdk.h"

#include "Hardware.hpp"

class DynamixelKinematics: public hardware::Kinematics {
  public:
    DynamixelKinematics();
    ~DynamixelKinematics();
    void set_joint_control_mode(hardware::Joint joint, hardware::JointControlMode mode, double max_acceleration, double max_rpm, double millis);
    void set_joint_position(hardware::Joint joint, double degrees);
    void set_joint_speed(hardware::Joint joint, double rpm);
    double get_joint_position(hardware::Joint joint);

  private:
    dynamixel::PortHandler *port_handler;
    dynamixel::PacketHandler *packet_handler;

    void enable_torque(int id, bool enable);
};

#endif  // NAMINUKAS_BRAIN_DYNAMIXEL_KINEMATICS_H_
