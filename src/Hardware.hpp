#ifndef SRC_HARDWARE_HPP_
#define SRC_HARDWARE_HPP_

#include <functional>
#include <vector>

namespace hardware {

enum class Joint { left_wheel, right_wheel, left_ankle, right_ankle };

enum class JointControlMode { off, position, velocity, time, pwm };

struct JointPosition {
  Joint joint;
  double degrees;
};

struct JointSpeed {
  Joint joint;
  double rpm;
};

struct JointPWM {
  Joint joint;

  /**
   * Number between -1 and 1.
   * 1 = 100% throttle, 0 = 0% throttle, -1 = -100% throttle.
   */
  double pwm;
};

class JointState {
 public:
  /**
   * In degrees.
   */
  double position;

  /**
   * Revolutions per minute.
   */
  double rpm;

  /**
   * In amperes.
   */
  double current;
};

class Kinematics {
 public:
  virtual void set_joint_control_mode(
    Joint joint, JointControlMode mode,
    double max_acceleration = 0, double max_rpm = 0, int acceleration_ms = 0, int total_ms = 0) = 0;
  virtual void set_joint_position(std::vector<JointPosition> positions) = 0;
  virtual void set_joint_speed(std::vector<JointSpeed> speeds) = 0;
  virtual void set_joint_pwm(std::vector<JointPWM> pwms) = 0;
  virtual std::vector<JointPosition> get_joint_position(std::vector<Joint> joints) = 0;
  virtual std::vector<JointSpeed> get_joint_speed(std::vector<Joint> joints) = 0;
  virtual std::vector<JointState> get_joint_state(std::vector<Joint> joints) = 0;
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
