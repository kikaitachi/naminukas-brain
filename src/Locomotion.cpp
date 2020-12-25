#include "Locomotion.hpp"

Locomotion::Locomotion(int control_loop_frequency)
    : control_loop_frequency(control_loop_frequency) {
  control_loop_nanos = 1000000000 / control_loop_frequency;
}

Locomotion::~Locomotion() {
}

void Locomotion::control_loop() {
}

void Locomotion::start() {
  if (stopped) {
    on_start();
    stopped = false;
    control_loop_thread = new std::thread([&]() {
      struct timespec last_control_loop_time;
      clock_gettime(CLOCK_MONOTONIC, &last_control_loop_time);
      while (!stopped) {
        control_loop();
        struct timespec now;
        clock_gettime(CLOCK_MONOTONIC, &now);
        uint64_t elapsed_nanos = (now.tv_sec - last_control_loop_time.tv_sec) * 1000000000 + (now.tv_nsec - last_control_loop_time.tv_nsec);
        last_control_loop_time = now;
        std::this_thread::sleep_for(std::chrono::nanoseconds(control_loop_nanos - elapsed_nanos));
      }
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

void Locomotion::up(bool key_down, std::set<std::string> modifiers) {
}

void Locomotion::down(bool key_down, std::set<std::string> modifiers) {
}

void Locomotion::left(bool key_down, std::set<std::string> modifiers) {
}

void Locomotion::right(bool key_down, std::set<std::string> modifiers) {
}
