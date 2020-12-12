#ifndef NAMINUKAS_BRAIN_BEAGLEBONE_BLUE_IMU_H_
#define NAMINUKAS_BRAIN_BEAGLEBONE_BLUE_IMU_H_

#include <robotcontrol.h>

#include "IMU.hpp"

class BeagleBoneBlueIMU: public IMU {
  public:
    BeagleBoneBlueIMU();
    virtual ~BeagleBoneBlueIMU();
    float get_yaw();
    float get_pitch();
    float get_roll();
};

#endif  // NAMINUKAS_BRAIN_BEAGLEBONE_BLUE_IMU_H_
