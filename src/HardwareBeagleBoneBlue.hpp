#ifndef SRC_HARDWAREBEAGLEBONEBLUE_HPP_
#define SRC_HARDWAREBEAGLEBONEBLUE_HPP_

#include "Hardware.hpp"

class BeagleBoneBlueIMU: public hardware::IMU {
 public:
  BeagleBoneBlueIMU();
  virtual ~BeagleBoneBlueIMU();
  double get_yaw();
  double get_pitch();
  double get_roll();
};

#endif  // SRC_HARDWAREBEAGLEBONEBLUE_HPP_
