#include "LocomotionUnicycle.hpp"

LocomotionUnicycle::LocomotionUnicycle(hardware::Kinematics& kinematics, IMU& imu)
    : Locomotion(200), kinematics(kinematics), imu(imu) {
  //
}

std::string LocomotionUnicycle::name() {
  return "Unicycle";
}

void LocomotionUnicycle::control_loop() {
  // TODO: implement
}

void LocomotionUnicycle::on_start() {
  // TODO: implement
}

void LocomotionUnicycle::on_stop() {
  // TODO: implement
}

void LocomotionUnicycle::up(bool key_down) {
  // TODO: implement
}

void LocomotionUnicycle::down(bool key_down) {
  // TODO: implement
}
