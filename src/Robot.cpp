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

  telemetry::ItemAction* caterpillar = new telemetry::ItemAction(
    mode->getId(), "Caterpilar", "Digit4", [=]() {
      this->mode->update("caterpilar");
    });
  telemetryItems.add_item(caterpillar);
}

void Robot::lockState() {
  stateMutex.lock();
}

void Robot::unlockState() {
  stateMutex.unlock();
}
