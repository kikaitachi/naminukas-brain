#include "LocomotionCaterpillar.hpp"

LocomotionCaterpillar::LocomotionCaterpillar(hardware::Kinematics& kinematics)
    : Locomotion(10), kinematics(kinematics) {
}

std::string LocomotionCaterpillar::name() {
  return "Caterpillar";
}

void LocomotionCaterpillar::control_loop() {
  //
}

void LocomotionCaterpillar::on_start() {
  //
}

void LocomotionCaterpillar::on_stop() {
  //
}

void LocomotionCaterpillar::up(bool key_down, std::set<std::string>& modifiers) {
  //
}

void LocomotionCaterpillar::down(bool key_down, std::set<std::string>& modifiers) {
  //
}

void LocomotionCaterpillar::left(bool key_down, std::set<std::string>& modifiers) {
  //
}

void LocomotionCaterpillar::right(bool key_down, std::set<std::string>& modifiers) {
  //
}
