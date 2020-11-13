#ifndef NAMINUKAS_BRAIN_DYNAMIXEL_UTIL_H_
#define NAMINUKAS_BRAIN_DYNAMIXEL_UTIL_H_

#include <initializer_list>
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
};

class DynamixelXM430W350: public DynamixelModel {
  public:
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
};

class DynamixelConnection {
  public:
    DynamixelConnection(std::string device, float protocol, int baudrate);
    ~DynamixelConnection();
    bool write(DynamixelControlItem item, std::initializer_list<DynamixelControlValue> values);
    std::vector<int> read(DynamixelControlItem item, std::initializer_list<int> ids);

  private:
    dynamixel::PortHandler *port_handler;
    dynamixel::PacketHandler *packet_handler;
};

#endif  // NAMINUKAS_BRAIN_DYNAMIXEL_UTIL_H_
