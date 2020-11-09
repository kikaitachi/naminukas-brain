#ifndef NAMINUKAS_BRAIN_IMU_H_
#define NAMINUKAS_BRAIN_IMU_H_

class IMU {
  public:
    virtual double yaw() = 0;
    virtual double pitch() = 0;
    virtual double roll() = 0;
};

#endif  // NAMINUKAS_BRAIN_IMU_H_
