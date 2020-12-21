#include "LocomotionIdle.hpp"

LocomotionIdle::LocomotionIdle(hardware::Kinematics& kinematics)
    : Locomotion(10), kinematics(kinematics) {
}

std::string LocomotionIdle::name() {
  return "Idle";
}
