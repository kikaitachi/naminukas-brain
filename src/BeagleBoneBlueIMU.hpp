#ifndef SRC_BEAGLEBONEBLUEIMU_HPP_
#define SRC_BEAGLEBONEBLUEIMU_HPP_

#include <robotcontrol.h>

#include "IMU.hpp"

class BeagleBoneBlueIMU: public IMU {
 public:
  BeagleBoneBlueIMU();
  virtual ~BeagleBoneBlueIMU();
  double get_yaw();
  double get_pitch();
  double get_roll();
};

#endif  // SRC_BEAGLEBONEBLUEIMU_HPP_
