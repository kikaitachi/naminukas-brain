#include "Robot.hpp"

std::mutex Robot::stateMutex;

Robot::Robot(telemetry::Items& telemetryItems) : telemetryItems(telemetryItems) {
  mode = new telemetry::ItemString(telemetry::ROOT_ITEM_ID, "Mode of operation", "idle");
  telemetryItems.add_item(mode);

  telemetry::ItemAction* idle = new telemetry::ItemAction(
    mode->getId(), "Idle", "Escape", [=]() {
      mode->update("idle");
    });
  telemetryItems.add_item(idle);

  telemetry::ItemAction* walk = new telemetry::ItemAction(
    mode->getId(), "Walk", "Digit1", [=]() {
      mode->update("walk");
    });
  telemetryItems.add_item(walk);

  telemetry::ItemAction* balance = new telemetry::ItemAction(
    mode->getId(), "Balance", "Digit2", [=]() {
      mode->update("balance");
    });
  telemetryItems.add_item(balance);

  telemetry::ItemAction* tilt_drive = new telemetry::ItemAction(
    mode->getId(), "Tilt drive", "Digit3", [=]() {
      mode->update("tilt drive");
    });
  telemetryItems.add_item(tilt_drive);

  telemetry::ItemAction* arm = new telemetry::ItemAction(
    mode->getId(), "Arm (velocity)", "Digit4", [=]() {
      this->mode->update("arm");
    });
  telemetryItems.add_item(arm);

  telemetry::ItemAction* caterpillar = new telemetry::ItemAction(
    mode->getId(), "Caterpilar", "Digit5", [=]() {
      this->mode->update("caterpilar");
    });
  telemetryItems.add_item(caterpillar);

  telemetry::ItemAction* up = new telemetry::ItemAction(
    mode->getId(), "Up", "ArrowUp", [=]() {
      // TODO: implement
    });
  telemetryItems.add_item(up);

  telemetry::ItemAction* down = new telemetry::ItemAction(
    mode->getId(), "Down", "ArrowDown", [=]() {
      // TODO: implement
    });
  telemetryItems.add_item(down);

  telemetry::ItemAction* left = new telemetry::ItemAction(
    mode->getId(), "Left", "ArrowLeft", [=]() {
      // TODO: implement
    });
  telemetryItems.add_item(left);

  telemetry::ItemAction* right = new telemetry::ItemAction(
    mode->getId(), "Right", "ArrowRight", [=]() {
      // TODO: implement
    });
  telemetryItems.add_item(right);
}

void Robot::lockState() {
  stateMutex.lock();
}

void Robot::unlockState() {
  stateMutex.unlock();
}
