#ifndef NAMINUKAS_BRAIN_BEAGLEBONE_BLUE_IMU_H_
#define NAMINUKAS_BRAIN_BEAGLEBONE_BLUE_IMU_H_

#include <robotcontrol.h>

#include "IMU.hpp"

class BeagleBoneBlueIMU: public IMU {
  public:
    BeagleBoneBlueIMU();
    virtual ~BeagleBoneBlueIMU();
    double yaw();
    double pitch();
    double roll();

  private:
    rc_mpu_data_t mpu_data;
};

#endif  // NAMINUKAS_BRAIN_BEAGLEBONE_BLUE_IMU_H_
