#ifndef SRC_DYNAMIXELUTIL_HPP_
#define SRC_DYNAMIXELUTIL_HPP_

#include <mutex>
#include <string>
#include <vector>

#include "dynamixel_sdk.h"

#define DYNAMIXEL_DRIVE_MODE_VELOCITY_BASED 0
#define DYNAMIXEL_DRIVE_MODE_TIME_BASED 4

#define DYNAMIXEL_OPERATING_MODE_CURRENT 0
#define DYNAMIXEL_OPERATING_MODE_VELOCITY 1
#define DYNAMIXEL_OPERATING_MODE_POSITION 3
#define DYNAMIXEL_OPERATING_MODE_EXTENDED_POSITION 4
#define DYNAMIXEL_OPERATING_MODE_CURRENT_BASED_POSITION 5
#define DYNAMIXEL_OPERATING_MODE_PWM 16

#define DYNAMIXEL_PROFILE_IN_PROGRESS 2

class DynamixelControlItem {
 public:
  int address, size;
  DynamixelControlItem(int address, int size);
};

class DynamixelControlValue {
 public:
  int id, value;
  DynamixelControlValue(int id, int value);
};

class DynamixelModel {
 public:
  virtual int positions_per_rotation() = 0;
  virtual int rpm_to_value(double rpm) = 0;
  virtual double value_to_rpm(int value) = 0;
  virtual int rpm2_to_value(double rpm2) = 0;
  virtual double value_to_current(int value) = 0;
  virtual int pwm_to_value(double pwm) = 0;

  virtual DynamixelControlItem drive_mode() = 0;
  virtual DynamixelControlItem operating_mode() = 0;
  virtual DynamixelControlItem torque_enable() = 0;
  virtual DynamixelControlItem goal_pwm() = 0;
  virtual DynamixelControlItem goal_current() = 0;
  virtual DynamixelControlItem goal_velocity() = 0;
  virtual DynamixelControlItem profile_acceleration() = 0;
  virtual DynamixelControlItem profile_velocity() = 0;
  virtual DynamixelControlItem goal_position() = 0;
  virtual DynamixelControlItem moving() = 0;
  virtual DynamixelControlItem moving_status() = 0;
  virtual DynamixelControlItem present_current() = 0;
  virtual DynamixelControlItem present_velocity() = 0;
  virtual DynamixelControlItem present_position() = 0;
  virtual DynamixelControlItem voltage() = 0;
  virtual DynamixelControlItem temperature() = 0;
  virtual DynamixelControlItem indirect_address(int offset) = 0;
};

class DynamixelXM430W350: public DynamixelModel {
 public:
  int positions_per_rotation();
  int rpm_to_value(double rpm);
  double value_to_rpm(int value);
  int rpm2_to_value(double rpm2);
  double value_to_current(int value);
  int pwm_to_value(double pwm);

  DynamixelControlItem drive_mode();
  DynamixelControlItem operating_mode();
  DynamixelControlItem torque_enable();
  DynamixelControlItem goal_pwm();
  DynamixelControlItem goal_current();
  DynamixelControlItem goal_velocity();
  DynamixelControlItem profile_acceleration();
  DynamixelControlItem profile_velocity();
  DynamixelControlItem goal_position();
  DynamixelControlItem moving();
  DynamixelControlItem moving_status();
  DynamixelControlItem present_current();
  DynamixelControlItem present_velocity();
  DynamixelControlItem present_position();
  DynamixelControlItem voltage();
  DynamixelControlItem temperature();
  DynamixelControlItem indirect_address(int offset);
};

class DynamixelConnection {
 public:
  DynamixelConnection(std::string device, float protocol, int baudrate);
  ~DynamixelConnection();
  bool write(DynamixelControlItem item, std::vector<DynamixelControlValue> values);
  std::vector<int> read(DynamixelControlItem item, std::vector<int> ids);
  std::vector<std::vector<int>> read(
    int address, std::vector<int> sizes, std::vector<int> ids);
 private:
  dynamixel::PortHandler *port_handler;
  dynamixel::PacketHandler *packet_handler;
  std::mutex mutex;
};

#endif  // SRC_DYNAMIXELUTIL_HPP_
