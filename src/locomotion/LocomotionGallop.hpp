#ifndef SRC_LOCOMOTION_LOCOMOTIONGALLOP_HPP_
#define SRC_LOCOMOTION_LOCOMOTIONGALLOP_HPP_

#include <string>
#include <vector>

#include "Locomotion.hpp"

class LocomotionGallop: public Locomotion {
 public:
  LocomotionGallop(hardware::Kinematics& kinematics);
  std::string name();
  Pose control_loop(Pose pose);
  void on_start();
 private:
  hardware::Kinematics& kinematics;
  std::vector<hardware::JointPosition> initial_pos;
  bool going_up;
};

#endif  // SRC_LOCOMOTION_LOCOMOTIONGALLOP_HPP_
