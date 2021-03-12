#include <chrono>
#include <fstream>
#include <set>

#include "locomotion/LocomotionGallop.hpp"
#include "locomotion/LocomotionHang.hpp"
#include "locomotion/LocomotionIdle.hpp"
#include "locomotion/LocomotionPole.hpp"
#include "locomotion/LocomotionPoleGestures.hpp"
#include "locomotion/LocomotionSegway.hpp"
#include "locomotion/LocomotionSki.hpp"
#include "locomotion/LocomotionTail.hpp"
#include "locomotion/LocomotionUnicycle.hpp"
#include "locomotion/LocomotionWaddle.hpp"
#include "Logger.hpp"
#include "Robot.hpp"

void Robot::add_locomotion(Locomotion* locomotion, std::string key) {
  locomotion_modes.push_back(locomotion);
  telemetryItems.add_item(std::make_shared<telemetry::ItemCommand>(
    mode->getId(), locomotion->name(), key, [=](int value, std::set<std::string>& modifiers) {
      if (value == 1) {
        logger::debug("Stopping locomotion: %s", current_locomotion_mode->name().c_str());
        current_locomotion_mode->stop();
        current_locomotion_mode = locomotion;
        logger::debug("Starting locomotion: %s", current_locomotion_mode->name().c_str());
        current_locomotion_mode->start();
        mode->update(current_locomotion_mode->name());
      }
    }, std::initializer_list<std::string>{ }));
}

Robot::Robot(
  telemetry::Items& telemetryItems,
  hardware::IMU& imu,
  hardware::Kinematics& kinematics,
  hardware::Pneumatics& pneumatics,
  Model& model,
  PointCloud& camera)
    : telemetryItems(telemetryItems), imu(imu), kinematics(kinematics), pneumatics(pneumatics), model(model) {
  LocomotionIdle* locomotion_idle = new LocomotionIdle(kinematics);

  current_locomotion_mode = locomotion_idle;
  mode = std::make_shared<telemetry::ItemString>(
    telemetry::ROOT_ITEM_ID, "Mode of operation", current_locomotion_mode->name());
  telemetryItems.add_item(mode);

  add_locomotion(locomotion_idle, "Escape");
  add_locomotion(new LocomotionPole(kinematics, model), "Digit1");
  add_locomotion(new LocomotionSegway(kinematics, imu), "Digit2");
  add_locomotion(new LocomotionSki(kinematics, imu), "Digit3");
  add_locomotion(new LocomotionTail(kinematics, imu), "Digit4");
  add_locomotion(new LocomotionWaddle(kinematics), "Digit5");
  add_locomotion(new LocomotionUnicycle(kinematics, imu), "Digit6");
  add_locomotion(new LocomotionPoleGestures(kinematics, model, camera), "Digit7");
  add_locomotion(new LocomotionHang(kinematics), "Digit8");
  add_locomotion(new LocomotionGallop(kinematics), "Digit9");

  telemetryItems.add_item(std::make_shared<telemetry::ItemCommand>(
    mode->getId(), "Up", "ArrowUp", [&](int value, std::set<std::string>& modifiers) {
      current_locomotion_mode->up(value == 1, modifiers);
    }, std::initializer_list<std::string>{ "Control", "Shift" }));

  telemetryItems.add_item(std::make_shared<telemetry::ItemCommand>(
    mode->getId(), "Down", "ArrowDown", [&](int value, std::set<std::string>& modifiers) {
      current_locomotion_mode->down(value == 1, modifiers);
    }, std::initializer_list<std::string>{ "Control", "Shift" }));

  telemetryItems.add_item(std::make_shared<telemetry::ItemCommand>(
    mode->getId(), "Left", "ArrowLeft", [&](int value, std::set<std::string>& modifiers) {
      current_locomotion_mode->left(value == 1, modifiers);
    }, std::initializer_list<std::string>{ "Control", "Shift" }));

  telemetryItems.add_item(std::make_shared<telemetry::ItemCommand>(
    mode->getId(), "Right", "ArrowRight", [&](int value, std::set<std::string>& modifiers) {
      current_locomotion_mode->right(value == 1, modifiers);
    }, std::initializer_list<std::string>{ "Control", "Shift" }));

  telemetryItems.add_item(std::make_shared<telemetry::ItemCommand>(
    mode->getId(), "Toggle pump", "KeyP", [&](int value, std::set<std::string>& modifiers) {
      if (value == 1) {
        pump_state = !pump_state;
        pneumatics.vacuum_pump_on(pump_state);
      }
    }, std::initializer_list<std::string>{ }));

  telemetryItems.add_item(std::make_shared<telemetry::ItemCommand>(
    mode->getId(), "Sound", "KeyK", [&](int value, std::set<std::string>& modifiers) {
      if (value == 1) {
        play();
      }
    }, std::initializer_list<std::string>{ }));
}

void Robot::play() {
  // Super experimental
  kinematics.set_joint_control_mode(hardware::Joint::right_ankle, hardware::JointControlMode::position);
  kinematics.set_joint_control_mode(hardware::Joint::right_wheel, hardware::JointControlMode::velocity);

  std::ifstream is;
  is.open("/tmp/test.raw", std::ios::binary);
  is.seekg(0, std::ios::end);
  int length = is.tellg();
  is.seekg(0, std::ios::beg);
  int8_t buffer[length]; // NOLINT
  is.read(reinterpret_cast<char*>(buffer), length);
  is.close();
  int sample_rate = 8000;
  int nanos_per_sample = 1000000000 / sample_rate;

  logger::info("Read %d samples", length);
  int count = 0;
  std::chrono::time_point<std::chrono::high_resolution_clock> start_time =
        std::chrono::high_resolution_clock::now();
  for ( ; ; ) {
    std::chrono::time_point<std::chrono::high_resolution_clock> now =
        std::chrono::high_resolution_clock::now();
    uint64_t elapsed_nanos = std::chrono::duration_cast<std::chrono::nanoseconds>
      (now - start_time).count();
    int i = elapsed_nanos / nanos_per_sample;
    if (i > sample_rate * 10) {
      break;
    }
    double speed = 0 + buffer[i] / 2.0;
    kinematics.set_joint_speed({ { hardware::Joint::right_wheel, speed } });
    count++;
  }
  kinematics.set_joint_speed({ { hardware::Joint::right_wheel, 0 } });

  kinematics.set_joint_control_mode(hardware::Joint::right_ankle, hardware::JointControlMode::off);
  kinematics.set_joint_control_mode(hardware::Joint::right_wheel, hardware::JointControlMode::off);

  logger::info("Over: %d", count);
}

Robot::~Robot() {
  for (Locomotion* locomotion : locomotion_modes) {
    delete locomotion;
  }
  locomotion_modes.clear();
}
