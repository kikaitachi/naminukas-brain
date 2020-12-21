#include "Locomotion.hpp"

Locomotion::Locomotion(int control_loop_frequency)
    : control_loop_frequency(control_loop_frequency) {
}

Locomotion::~Locomotion() {
}

void Locomotion::control_loop() {
}

void Locomotion::start() {
  if (stopped) {
    on_start();
    control_loop_thread = new std::thread([&]() {
      control_loop();
      std::this_thread::sleep_for(std::chrono::milliseconds(1000 / control_loop_frequency));
    });
  }
}

void Locomotion::stop() {
  if (!stopped) {
    on_stop();
    stopped = true;
    control_loop_thread->join();
    delete control_loop_thread;
  }
}

void Locomotion::on_start() {
}

void Locomotion::on_stop() {
}

void Locomotion::up(bool key_down) {
}

void Locomotion::down(bool key_down) {
}

void Locomotion::left(bool key_down) {
}

void Locomotion::right(bool key_down) {
}
