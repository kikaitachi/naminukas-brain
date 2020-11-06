#ifndef NAMINUKAS_BRAIN_HARDWARE_H_
#define NAMINUKAS_BRAIN_HARDWARE_H_

namespace hardware {

  enum class Joint { left_wheel, right_wheel, left_ankle, right_ankle };

  enum class JointControlMode { position, velocity, time };

  class Kinematics {
    public:
      virtual void set_joint_control_mode(Joint joint, JointControlMode mode, double max_acceleration, double max_rpm, double millis) = 0;
      virtual void set_joint_position(Joint joint, double degrees) = 0;
      virtual void set_joint_speed(Joint joint, double rpm) = 0;
  };

  class Pneumatics {
    public:
      virtual void vacuum_pump_on(bool on) = 0;
      virtual void left_foot_vent(bool vent) = 0;
      virtual void right_foot_vent(bool vent) = 0;
  };
}

#endif  // NAMINUKAS_BRAIN_HARDWARE_H_
