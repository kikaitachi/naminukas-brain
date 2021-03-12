#ifndef SRC_LOCOMOTION_LOCOMOTIONIDLE_HPP_
#define SRC_LOCOMOTION_LOCOMOTIONIDLE_HPP_

#include <string>

#include "Locomotion.hpp"

class LocomotionIdle: public Locomotion {
 public:
  explicit LocomotionIdle(hardware::Kinematics& kinematics);
  std::string name();

 protected:
  hardware::Kinematics& kinematics;
};

#endif  // SRC_LOCOMOTION_LOCOMOTIONIDLE_HPP_
