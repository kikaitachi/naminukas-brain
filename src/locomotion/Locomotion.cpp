#include <chrono>

#include "../Logger.hpp"
#include "Locomotion.hpp"

Locomotion::Locomotion(int control_loop_frequency)
    : control_loop_frequency(control_loop_frequency) {
  control_loop_nanos = std::chrono::nanoseconds(1000000000 / control_loop_frequency);
}

Locomotion::~Locomotion() {
}

Pose Locomotion::control_loop(Pose pose) {
  return pose;
}

void Locomotion::start() {
  if (stopped) {
    on_start();
    stopped = false;
    control_loop_thread = new std::thread([&]() {
      std::chrono::time_point<std::chrono::high_resolution_clock> last_control_loop_time =
        std::chrono::high_resolution_clock::now();
      while (!stopped) {
        Pose pose = control_loop(pose);
        // TODO: construct [wheel] odometry / trajectory from poses
        // logger::debug("Pose: %f, %f", pose.location.x, pose.location.y);
        std::chrono::time_point<std::chrono::high_resolution_clock> now =
          std::chrono::high_resolution_clock::now();
        uint64_t elapsed_nanos = std::chrono::duration_cast<std::chrono::nanoseconds>
          (now - last_control_loop_time).count();
        int nanos_to_sleep = control_loop_nanos.count() - elapsed_nanos;
        if (nanos_to_sleep < 0) {
          logger::warn("%s: control loop overran by %dns", name().c_str(), -nanos_to_sleep);
        } else {
          std::this_thread::sleep_for(std::chrono::nanoseconds(nanos_to_sleep));
        }
        last_control_loop_time += control_loop_nanos;
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

void Locomotion::up(bool key_down, std::set<std::string>& modifiers) {
}

void Locomotion::down(bool key_down, std::set<std::string>& modifiers) {
}

void Locomotion::left(bool key_down, std::set<std::string>& modifiers) {
}

void Locomotion::right(bool key_down, std::set<std::string>& modifiers) {
}
