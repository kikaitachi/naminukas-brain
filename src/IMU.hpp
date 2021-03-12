#ifndef SRC_IMU_HPP_
#define SRC_IMU_HPP_

class IMU {
 public:
  virtual double get_yaw() = 0;
  virtual double get_pitch() = 0;
  virtual double get_roll() = 0;
};

#endif  // SRC_IMU_HPP_
