#include "LocomotionUnicycle.hpp"

LocomotionUnicycle::LocomotionUnicycle(hardware::Kinematics& kinematics, IMU& imu)
    : kinematics(kinematics), imu(imu) {
  //
}

std::string LocomotionUnicycle::name() {
  return "Unicycle";
}

void LocomotionUnicycle::start() {
  // TODO: implement
}

void LocomotionUnicycle::stop() {
  // TODO: implement
}

void LocomotionUnicycle::up(bool key_down) {
  // TODO: implement
}

void LocomotionUnicycle::down(bool key_down) {
  // TODO: implement
}
