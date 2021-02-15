#include "locomotion/LocomotionGallop.hpp"
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

Robot::Robot(telemetry::Items& telemetryItems, IMU& imu, hardware::Kinematics& kinematics, Model& model, PointCloud& camera)
    : telemetryItems(telemetryItems), imu(imu), kinematics(kinematics), model(model) {
  LocomotionIdle* locomotion_idle = new LocomotionIdle(kinematics);

  current_locomotion_mode = locomotion_idle;
  mode = std::make_shared<telemetry::ItemString>(telemetry::ROOT_ITEM_ID, "Mode of operation", current_locomotion_mode->name());
  telemetryItems.add_item(mode);

  add_locomotion(locomotion_idle, "Escape");
  add_locomotion(new LocomotionPole(kinematics, model), "Digit1");
  add_locomotion(new LocomotionSegway(kinematics, imu), "Digit2");
  add_locomotion(new LocomotionSki(kinematics, imu), "Digit3");
  add_locomotion(new LocomotionTail(kinematics, imu), "Digit4");
  add_locomotion(new LocomotionWaddle(kinematics), "Digit5");
  add_locomotion(new LocomotionUnicycle(kinematics, imu), "Digit6");
  add_locomotion(new LocomotionPoleGestures(kinematics, model, camera), "Digit7");
  add_locomotion(new LocomotionGallop(kinematics), "Digit8");

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
}

Robot::~Robot() {
  for (Locomotion* locomotion : locomotion_modes) {
    delete locomotion;
  }
  locomotion_modes.clear();
}
