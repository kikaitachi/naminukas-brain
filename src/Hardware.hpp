#ifndef NAMINUKAS_BRAIN_HARDWARE_H_
#define NAMINUKAS_BRAIN_HARDWARE_H_

#include <vector>

namespace hardware {

  enum class Joint { left_wheel, right_wheel, left_ankle, right_ankle };

  enum class JointControlMode { off, position, velocity, time };

  class JointPosition {
    public:
      Joint joint;
      double degrees;
  };

  class JointSpeed {
    public:
      Joint joint;
      double rpm;
  };

  class Kinematics {
    public:
      virtual void set_joint_control_mode(
        Joint joint, JointControlMode mode, double max_acceleration = 0, double max_rpm = 0, double millis = 0) = 0;
      virtual void set_joint_position(std::vector<JointPosition> positions) = 0;
      virtual void set_joint_speed(std::vector<JointSpeed> speeds) = 0;
      virtual double get_joint_position(Joint joint) = 0;
  };

  class Pneumatics {
    public:
      virtual void vacuum_pump_on(bool on) = 0;
      virtual void left_foot_vent(bool vent) = 0;
      virtual void right_foot_vent(bool vent) = 0;
  };
}

#endif  // NAMINUKAS_BRAIN_HARDWARE_H_
