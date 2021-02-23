#ifndef SRC_HARDWARE_HPP_
#define SRC_HARDWARE_HPP_

#include <functional>
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
    Joint joint, JointControlMode mode,
    double max_acceleration = 0, double max_rpm = 0, int acceleration_ms = 0, int total_ms = 0) = 0;
  virtual void set_joint_position(std::vector<JointPosition> positions) = 0;
  virtual void set_joint_speed(std::vector<JointSpeed> speeds) = 0;
  virtual std::vector<JointPosition> get_joint_position(std::vector<Joint> joints) = 0;
  virtual bool reached_destination(std::vector<Joint> joints) = 0;
  void add_position_listener(std::function<void(std::vector<JointPosition>&)> listener);
 protected:
  std::vector<std::function<void(std::vector<JointPosition>&)>> position_listeners;
};

class Pneumatics {
 public:
  virtual void vacuum_pump_on(bool on) = 0;
  virtual void left_foot_vent(bool vent) = 0;
  virtual void right_foot_vent(bool vent) = 0;
};

}

#endif  // SRC_HARDWARE_HPP_
