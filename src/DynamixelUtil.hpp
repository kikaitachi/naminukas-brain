#ifndef NAMINUKAS_BRAIN_DYNAMIXEL_UTIL_H_
#define NAMINUKAS_BRAIN_DYNAMIXEL_UTIL_H_

#include <initializer_list>
#include <string>
#include <vector>

#include "dynamixel_sdk.h"

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
};

class DynamixelXM430W350: public DynamixelModel {
  public:
    DynamixelControlItem drive_mode();
    DynamixelControlItem operating_mode();
    DynamixelControlItem torque_enable();
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
