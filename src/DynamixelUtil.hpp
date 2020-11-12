#ifndef NAMINUKAS_BRAIN_DYNAMIXEL_UTIL_H_
#define NAMINUKAS_BRAIN_DYNAMIXEL_UTIL_H_

class DynamixelControlItem {
  public:
    int address, size;
    DynamixelControlItem(int address, int size);
};

class DynamixelModel {
  public:
    virtual DynamixelControlItem torque_enable() = 0;
};

class DynamixelXM430W350: public DynamixelModel {
  public:
    DynamixelControlItem torque_enable();
};

#endif  // NAMINUKAS_BRAIN_DYNAMIXEL_UTIL_H_
