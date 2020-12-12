#include "LocomotionIdle.hpp"
#include "LocomotionPole.hpp"
#include "LocomotionPoleGestures.hpp"
#include "LocomotionSki.hpp"
#include "Logger.hpp"
#include "Robot.hpp"

void Robot::add_locomotion(Locomotion* locomotion, std::string key) {
  telemetryItems.add_item(new telemetry::ItemCommand(
    mode->getId(), locomotion->name(), key, [=](int value) {
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
  model = new Model(telemetryItems);

  LocomotionIdle* locomotion_idle = new LocomotionIdle(kinematics);
  LocomotionPole* locomotion_pole = new LocomotionPole(kinematics, *model);
  LocomotionPoleGestures* locomotion_pole_gestures = new LocomotionPoleGestures(kinematics, *model, camera);
  LocomotionSki* locomotion_ski = new LocomotionSki(kinematics, imu);
  current_locomotion_mode = locomotion_idle;

  mode = new telemetry::ItemString(telemetry::ROOT_ITEM_ID, "Mode of operation", current_locomotion_mode->name());
  telemetryItems.add_item(mode);

  add_locomotion(locomotion_idle, "Escape");
  add_locomotion(locomotion_pole, "Digit1");
  add_locomotion(locomotion_pole_gestures, "Digit2");
  add_locomotion(locomotion_ski, "Digit3");

  /*telemetry::ItemCommand* walk = new telemetry::ItemCommand(
    mode->getId(), "Walk", "Digit1", [&]() {
      kinematics.set_joint_control_mode(hardware::Joint::left_wheel, hardware::JointControlMode::position);
      kinematics.set_joint_control_mode(hardware::Joint::left_ankle, hardware::JointControlMode::position);
      kinematics.set_joint_control_mode(hardware::Joint::right_ankle, hardware::JointControlMode::position);
      kinematics.set_joint_control_mode(hardware::Joint::right_wheel, hardware::JointControlMode::position);
      mode->update("walk");
    });
  telemetryItems.add_item(walk);

  telemetry::ItemCommand* balance = new telemetry::ItemCommand(
    mode->getId(), "Balance", "Digit2", [&]() {
      kinematics.set_joint_control_mode(hardware::Joint::left_wheel, hardware::JointControlMode::position);
      kinematics.set_joint_control_mode(hardware::Joint::left_ankle, hardware::JointControlMode::position);
      kinematics.set_joint_control_mode(hardware::Joint::right_ankle, hardware::JointControlMode::position);
      kinematics.set_joint_control_mode(hardware::Joint::right_wheel, hardware::JointControlMode::position);
      mode->update("balance");
    });
  telemetryItems.add_item(balance);

  telemetry::ItemCommand* arm = new telemetry::ItemCommand(
    mode->getId(), "Arm (velocity)", "Digit4", [&]() {
      kinematics.set_joint_control_mode(hardware::Joint::left_wheel, hardware::JointControlMode::velocity);
      kinematics.set_joint_control_mode(hardware::Joint::left_ankle, hardware::JointControlMode::velocity);
      kinematics.set_joint_control_mode(hardware::Joint::right_ankle, hardware::JointControlMode::velocity);
      kinematics.set_joint_control_mode(hardware::Joint::right_wheel, hardware::JointControlMode::velocity);
      this->mode->update("arm");
    });
  telemetryItems.add_item(arm);

  telemetry::ItemCommand* caterpillar = new telemetry::ItemCommand(
    mode->getId(), "Caterpilar", "Digit5", [&]() {
      kinematics.set_joint_control_mode(hardware::Joint::left_wheel, hardware::JointControlMode::position);
      kinematics.set_joint_control_mode(hardware::Joint::left_ankle, hardware::JointControlMode::position);
      kinematics.set_joint_control_mode(hardware::Joint::right_ankle, hardware::JointControlMode::position);
      kinematics.set_joint_control_mode(hardware::Joint::right_wheel, hardware::JointControlMode::position);
      this->mode->update("caterpilar");
    });
  telemetryItems.add_item(caterpillar);*/

  telemetry::ItemCommand* up = new telemetry::ItemCommand(
    mode->getId(), "Up", "ArrowUp", [&](int value) {
      current_locomotion_mode->up(value == 1);
    }, { /*"ControlLeft", "ControlRight",*/ "AltLeft", "AltRight"/*, "ShiftLeft", "ShiftRight"*/ });
  telemetryItems.add_item(up);

  telemetry::ItemCommand* down = new telemetry::ItemCommand(
    mode->getId(), "Down", "ArrowDown", [&](int value) {
      current_locomotion_mode->down(value == 1);
    }, { /*"ControlLeft", "ControlRight",*/ "AltLeft", "AltRight"/*, "ShiftLeft", "ShiftRight"*/ });
  telemetryItems.add_item(down);

  telemetry::ItemCommand* left = new telemetry::ItemCommand(
    mode->getId(), "Left", "ArrowLeft", [&](int value) {
      current_locomotion_mode->left(value == 1);
    }, { /*"ControlLeft", "ControlRight",*/ "AltLeft", "AltRight"/*, "ShiftLeft", "ShiftRight"*/ });
  telemetryItems.add_item(left);

  telemetry::ItemCommand* right = new telemetry::ItemCommand(
    mode->getId(), "Right", "ArrowRight", [&](int value) {
      current_locomotion_mode->right(value == 1);
    }, { /*"ControlLeft", "ControlRight",*/ "AltLeft", "AltRight"/*, "ShiftLeft", "ShiftRight"*/ });
  telemetryItems.add_item(right);
}

Robot::~Robot() {
  delete model;
  // TODO: delete locomotion modes
}
