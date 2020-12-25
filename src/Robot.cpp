#include "LocomotionIdle.hpp"
#include "LocomotionPole.hpp"
#include "LocomotionPoleGestures.hpp"
#include "LocomotionSegway.hpp"
#include "LocomotionSki.hpp"
#include "LocomotionUnicycle.hpp"
#include "Logger.hpp"
#include "Robot.hpp"

void Robot::add_locomotion(Locomotion* locomotion, std::string key) {
  locomotion_modes.push_back(locomotion);
  telemetryItems.add_item(new telemetry::ItemCommand(
    mode->getId(), locomotion->name(), key, [=](int value, std::set<std::string> modifiers) {
      if (value == 1) {
        logger::debug("Stopping locomotion: %s", current_locomotion_mode->name().c_str());
        current_locomotion_mode->stop();
        current_locomotion_mode = locomotion;
        logger::debug("Starting locomotion: %s", current_locomotion_mode->name().c_str());
        current_locomotion_mode->start();
        mode->update(current_locomotion_mode->name());
      }
    }, { }));
}

Robot::Robot(telemetry::Items& telemetryItems, IMU& imu, hardware::Kinematics& kinematics, PointCloud& camera) :
    telemetryItems(telemetryItems), imu(imu), kinematics(kinematics) {
  model = new Model(telemetryItems, kinematics);

  LocomotionIdle* locomotion_idle = new LocomotionIdle(kinematics);

  current_locomotion_mode = locomotion_idle;
  mode = new telemetry::ItemString(telemetry::ROOT_ITEM_ID, "Mode of operation", current_locomotion_mode->name());
  telemetryItems.add_item(mode);

  add_locomotion(locomotion_idle, "Escape");
  add_locomotion(new LocomotionPole(kinematics, *model), "Digit1");
  add_locomotion(new LocomotionSegway(kinematics, imu), "Digit2");
  add_locomotion(new LocomotionSki(kinematics, imu), "Digit3");
  add_locomotion(new LocomotionUnicycle(kinematics, imu), "Digit4");
  add_locomotion(new LocomotionPoleGestures(kinematics, *model, camera), "Digit5");

  telemetry::ItemCommand* up = new telemetry::ItemCommand(
    mode->getId(), "Up", "ArrowUp", [&](int value, std::set<std::string> modifiers) {
      current_locomotion_mode->up(value == 1, modifiers);
    }, { "Control", "Shift" });
  telemetryItems.add_item(up);

  telemetry::ItemCommand* down = new telemetry::ItemCommand(
    mode->getId(), "Down", "ArrowDown", [&](int value, std::set<std::string> modifiers) {
      current_locomotion_mode->down(value == 1, modifiers);
    }, { "Control", "Shift" });
  telemetryItems.add_item(down);

  telemetry::ItemCommand* left = new telemetry::ItemCommand(
    mode->getId(), "Left", "ArrowLeft", [&](int value, std::set<std::string> modifiers) {
      current_locomotion_mode->left(value == 1, modifiers);
    }, { "Control", "Shift" });
  telemetryItems.add_item(left);

  telemetry::ItemCommand* right = new telemetry::ItemCommand(
    mode->getId(), "Right", "ArrowRight", [&](int value, std::set<std::string> modifiers) {
      current_locomotion_mode->right(value == 1, modifiers);
    }, { "Control", "Shift" });
  telemetryItems.add_item(right);
}

Robot::~Robot() {
  delete model;
  for (Locomotion* locomotion : locomotion_modes) {
    delete locomotion;
  }
  locomotion_modes.clear();
}
