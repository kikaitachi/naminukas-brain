#ifndef SRC_HARDWAREBEAGLEBONEBLUE_HPP_
#define SRC_HARDWAREBEAGLEBONEBLUE_HPP_

#include "Hardware.hpp"

class BeagleBoneBluePneumatics: public hardware::Pneumatics {
 public:
  BeagleBoneBluePneumatics();
  virtual ~BeagleBoneBluePneumatics();
  void vacuum_pump_on(bool on);
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
};

#endif  // SRC_HARDWAREBEAGLEBONEBLUE_HPP_
