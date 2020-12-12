#ifndef NAMINUKAS_BRAIN_IMU_H_
#define NAMINUKAS_BRAIN_IMU_H_

class IMU {
  public:
    virtual float get_yaw() = 0;
    virtual float get_pitch() = 0;
    virtual float get_roll() = 0;
};

#endif  // NAMINUKAS_BRAIN_IMU_H_
