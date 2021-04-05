#ifndef SRC_HARDWAREBEAGLEBONEBLUE_HPP_
#define SRC_HARDWAREBEAGLEBONEBLUE_HPP_

#include <fstream>

#include "Hardware.hpp"

class BeagleBoneBluePneumatics: public hardware::Pneumatics {
 public:
  BeagleBoneBluePneumatics();
  virtual ~BeagleBoneBluePneumatics();
  void set_vacuum_pump_speed(double speed);
  void left_foot_vent(bool vent);
  void right_foot_vent(bool vent);
};

class BeagleBoneBlueIMU: public hardware::IMU {
 public:
  BeagleBoneBlueIMU();
  virtual ~BeagleBoneBlueIMU();
  double get_yaw();
  double get_pitch();
  double get_roll();
  char get_tap_code();
};

#endif  // SRC_HARDWAREBEAGLEBONEBLUE_HPP_
