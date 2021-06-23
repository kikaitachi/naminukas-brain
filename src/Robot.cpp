#include <chrono>
#include <fstream>
#include <set>

#include "locomotion/LocomotionCaterpillar.hpp"
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
  hardware::Barometer& barometer,
  hardware::Kinematics& kinematics,
  hardware::Pneumatics& pneumatics,
  Model& model,
  PointCloud& camera,
  RCRadio& rc_radio)
    : telemetryItems(telemetryItems), imu(imu), barometer(barometer),
      kinematics(kinematics), pneumatics(pneumatics), model(model),
      rc_radio(rc_radio) {
  LocomotionIdle* locomotion_idle = new LocomotionIdle(kinematics);

  current_locomotion_mode = locomotion_idle;
  mode = std::make_shared<telemetry::ItemString>(
    telemetry::ROOT_ITEM_ID, "Mode of operation", current_locomotion_mode->name());
  telemetryItems.add_item(mode);

  add_locomotion(locomotion_idle, "Escape");
  add_locomotion(new LocomotionPole(kinematics, model, camera, imu), "Digit1");
  add_locomotion(new LocomotionSegway(kinematics, imu), "Digit2");
  add_locomotion(new LocomotionSki(kinematics, imu), "Digit3");
  add_locomotion(new LocomotionTail(kinematics, imu), "Digit4");
  add_locomotion(new LocomotionWaddle(kinematics), "Digit5");
  add_locomotion(new LocomotionUnicycle(kinematics, imu), "Digit6");
  add_locomotion(new LocomotionPoleGestures(kinematics, model, camera, imu), "Digit7");
  add_locomotion(new LocomotionHang(kinematics), "Digit8");
  add_locomotion(new LocomotionCaterpillar(kinematics), "Digit9");

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
        pneumatics.set_vacuum_pump_speed(pump_state ? 1 : 0);
      }
    }, std::initializer_list<std::string>{ }));

  telemetryItems.add_item(std::make_shared<telemetry::ItemCommand>(
    mode->getId(), "Sound", "KeyK", [&](int value, std::set<std::string>& modifiers) {
      if (value == 1) {
        play();
      }
    }, std::initializer_list<std::string>{ }));
  
  rc_radio.init(*this);
}

void Robot::play() {
  // Super experimental
  /*
  // kinematics.set_joint_control_mode(hardware::Joint::right_ankle, hardware::JointControlMode::position);
  // kinematics.set_joint_control_mode(hardware::Joint::right_wheel, hardware::JointControlMode::velocity);

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
    // double speed = 0 + buffer[i] / 2.0;
    // kinematics.set_joint_speed({ { hardware::Joint::right_wheel, speed } });
    double speed = 0.2 + buffer[i] / 1280.0;
    if (speed < 0) {
      logger::error("Negative speed: %f", speed);
    } else if (speed > 0.3) {
      logger::error("Speed too high: %f", speed);
    } else {
      pneumatics.set_vacuum_pump_speed(speed);
    }
    count++;
  }
  pneumatics.set_vacuum_pump_speed(0);
  // kinematics.set_joint_speed({ { hardware::Joint::right_wheel, 0 } });

  // kinematics.set_joint_control_mode(hardware::Joint::right_ankle, hardware::JointControlMode::off);
  // kinematics.set_joint_control_mode(hardware::Joint::right_wheel, hardware::JointControlMode::off);

  logger::info("Over: %d", count);
  */

  /*int gap = 10;
  int beat = 400;
  for (int i = 0; i < 10; i++) {
    pneumatics.set_vacuum_pump_speed(0.3);
    std::this_thread::sleep_for(std::chrono::milliseconds(beat));
    pneumatics.set_vacuum_pump_speed(0);
    std::this_thread::sleep_for(std::chrono::milliseconds(gap));
  }
  for (int i = 0; i < 10; i++) {
    pneumatics.set_vacuum_pump_speed(0.2);
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    pneumatics.set_vacuum_pump_speed(0.3);
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    pneumatics.set_vacuum_pump_speed(0.4);
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    pneumatics.set_vacuum_pump_speed(0);
    std::this_thread::sleep_for(std::chrono::milliseconds(gap));
  }

  pneumatics.set_vacuum_pump_speed(0);*/

  /*int frequency = 200;
  std::chrono::nanoseconds control_loop_nanos = std::chrono::nanoseconds(1000000000 / frequency);
  std::ofstream imu_log("/tmp/barometer.csv", std::ios::out);
  imu_log
    << "time,pressure"
    << std::endl;
  std::chrono::time_point<std::chrono::high_resolution_clock> last_control_loop_time =
      std::chrono::high_resolution_clock::now();
  std::chrono::time_point<std::chrono::high_resolution_clock> start = last_control_loop_time;
  for (int i = 0; i < frequency * 10; i++) {  // 10 seconds @ 200Hz
    imu_log
      << std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - start).count()
      << ','
      << barometer.get_pressure()
      << std::endl;
    std::chrono::time_point<std::chrono::high_resolution_clock> now =
      std::chrono::high_resolution_clock::now();
    uint64_t elapsed_nanos = std::chrono::duration_cast<std::chrono::nanoseconds>
      (now - last_control_loop_time).count();
    int nanos_to_sleep = control_loop_nanos.count() - elapsed_nanos;
    if (nanos_to_sleep < 0) {
      logger::warn("Barmometer sampling loop overran by %dns", -nanos_to_sleep);
    } else {
      std::this_thread::sleep_for(std::chrono::nanoseconds(nanos_to_sleep));
    }
    last_control_loop_time += control_loop_nanos;
  }*/

  logger::info("Left foot presure: %f", pneumatics.get_pressure_left());
}

Robot::~Robot() {
  for (Locomotion* locomotion : locomotion_modes) {
    delete locomotion;
  }
  locomotion_modes.clear();
}

void Robot::on_rc_radio_channel_change(int channel, int new_value) {
  // TODO: implement
}