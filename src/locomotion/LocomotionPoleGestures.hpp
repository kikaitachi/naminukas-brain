#ifndef SRC_LOCOMOTION_LOCOMOTIONPOLEGESTURES_HPP_
#define SRC_LOCOMOTION_LOCOMOTIONPOLEGESTURES_HPP_

#include <string>

#include "../Camera.hpp"
#include "LocomotionPole.hpp"

class LocomotionPoleGestures: public LocomotionPole {
 public:
  LocomotionPoleGestures(hardware::Kinematics& kinematics, Model& model, PointCloud& camera, hardware::IMU& imu);
  std::string name();
};

#endif  // SRC_LOCOMOTION_LOCOMOTIONPOLEGESTURES_HPP_
