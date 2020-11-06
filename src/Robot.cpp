#include "Robot.hpp"

std::mutex Robot::stateMutex;

Robot::Robot(telemetry::Items& telemetryItems, hardware::Kinematics& kinematics) :
    telemetryItems(telemetryItems), kinematics(kinematics) {
  mode = new telemetry::ItemString(telemetry::ROOT_ITEM_ID, "Mode of operation", "idle");
  telemetryItems.add_item(mode);

  telemetry::ItemCommand* idle = new telemetry::ItemCommand(
    mode->getId(), "Idle", "Escape", [=]() {
      mode->update("idle");
    });
  telemetryItems.add_item(idle);

  telemetry::ItemCommand* walk = new telemetry::ItemCommand(
    mode->getId(), "Walk", "Digit1", [=]() {
      mode->update("walk");
    });
  telemetryItems.add_item(walk);

  telemetry::ItemCommand* balance = new telemetry::ItemCommand(
    mode->getId(), "Balance", "Digit2", [=]() {
      mode->update("balance");
    });
  telemetryItems.add_item(balance);

  telemetry::ItemCommand* tilt_drive = new telemetry::ItemCommand(
    mode->getId(), "Tilt drive", "Digit3", [=]() {
      mode->update("tilt drive");
    });
  telemetryItems.add_item(tilt_drive);

  telemetry::ItemCommand* arm = new telemetry::ItemCommand(
    mode->getId(), "Arm (velocity)", "Digit4", [=]() {
      this->mode->update("arm");
    });
  telemetryItems.add_item(arm);

  telemetry::ItemCommand* caterpillar = new telemetry::ItemCommand(
    mode->getId(), "Caterpilar", "Digit5", [=]() {
      this->mode->update("caterpilar");
    });
  telemetryItems.add_item(caterpillar);

  telemetry::ItemCommand* up = new telemetry::ItemCommand(
    mode->getId(), "Up", "ArrowUp", [=]() {
      // TODO: implement
    });
  telemetryItems.add_item(up);

  telemetry::ItemCommand* down = new telemetry::ItemCommand(
    mode->getId(), "Down", "ArrowDown", [=]() {
      // TODO: implement
    });
  telemetryItems.add_item(down);

  telemetry::ItemCommand* left = new telemetry::ItemCommand(
    mode->getId(), "Left", "ArrowLeft", [=]() {
      // TODO: implement
    });
  telemetryItems.add_item(left);

  telemetry::ItemCommand* right = new telemetry::ItemCommand(
    mode->getId(), "Right", "ArrowRight", [=]() {
      // TODO: implement
    });
  telemetryItems.add_item(right);

  telemetry::ItemString* parts = new telemetry::ItemString(telemetry::ROOT_ITEM_ID, "Parts", "2");
  telemetryItems.add_item(parts);

  telemetry::ItemSTL* left_foot = new telemetry::ItemSTL(parts->getId(), "Left foot", "../model/suction-cup-connector.stl");
  telemetryItems.add_item(left_foot);

  telemetry::ItemSTL* right_foot = new telemetry::ItemSTL(parts->getId(), "Right foot", "../model/suction-cup-connector.stl");
  telemetryItems.add_item(right_foot);
}

void Robot::lockState() {
  stateMutex.lock();
}

void Robot::unlockState() {
  stateMutex.unlock();
}
