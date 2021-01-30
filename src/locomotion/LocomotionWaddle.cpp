#include "LocomotionWaddle.hpp"

LocomotionWaddle::LocomotionWaddle(hardware::Kinematics& kinematics)
    : Locomotion(10), kinematics(kinematics) {
}

std::string LocomotionWaddle::name() {
  return "Waddle";
}

void LocomotionWaddle::control_loop() {
  //
}

void LocomotionWaddle::on_start() {
  //
}

void LocomotionWaddle::on_stop() {
  //
}

void LocomotionWaddle::up(bool key_down, std::set<std::string>& modifiers) {
  //
}

void LocomotionWaddle::down(bool key_down, std::set<std::string>& modifiers) {
  //
}

void LocomotionWaddle::left(bool key_down, std::set<std::string>& modifiers) {
  //
}

void LocomotionWaddle::right(bool key_down, std::set<std::string>& modifiers) {
  //
}
