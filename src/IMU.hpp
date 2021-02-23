#ifndef SRC_IMU_HPP_
#define SRC_IMU_HPP_

class IMU {
 public:
  virtual float get_yaw() = 0;
  virtual float get_pitch() = 0;
  virtual float get_roll() = 0;
};

#endif  // SRC_IMU_HPP_
