#include "Logger.hpp"
#include "Robot.hpp"

class LocomotionIdle: public Locomotion {
  public:
    LocomotionIdle(hardware::Kinematics& kinematics) : kinematics(kinematics) {
    }

    std::string name() {
      return "Idle";
    }

    void start() {
      kinematics.set_joint_control_mode(hardware::Joint::left_wheel, hardware::JointControlMode::off);
      kinematics.set_joint_control_mode(hardware::Joint::left_ankle, hardware::JointControlMode::off);
      kinematics.set_joint_control_mode(hardware::Joint::right_ankle, hardware::JointControlMode::off);
      kinematics.set_joint_control_mode(hardware::Joint::right_wheel, hardware::JointControlMode::off);
      // TODO: add Pneumatics
    }

  protected:
    hardware::Kinematics& kinematics;
};

class LocomotionTiltDrive: public Locomotion {
  public:
    LocomotionTiltDrive(hardware::Kinematics& kinematics) : kinematics(kinematics) {
    }

    std::string name() {
      return "Tilt drive";
    }

    void start() {
      kinematics.set_joint_control_mode(hardware::Joint::left_wheel, hardware::JointControlMode::velocity);
      kinematics.set_joint_control_mode(hardware::Joint::left_ankle, hardware::JointControlMode::position);
      kinematics.set_joint_control_mode(hardware::Joint::right_ankle, hardware::JointControlMode::position);
      kinematics.set_joint_control_mode(hardware::Joint::right_wheel, hardware::JointControlMode::velocity);
      kinematics.set_joint_position({
        { hardware::Joint::left_ankle, initial_ankle_angle + 10 },
        { hardware::Joint::right_ankle, initial_ankle_angle - 10 },
      });
    }

    void stop() {
      kinematics.set_joint_control_mode(hardware::Joint::left_wheel, hardware::JointControlMode::off);
      kinematics.set_joint_control_mode(hardware::Joint::left_ankle, hardware::JointControlMode::off);
      kinematics.set_joint_control_mode(hardware::Joint::right_ankle, hardware::JointControlMode::off);
      kinematics.set_joint_control_mode(hardware::Joint::right_wheel, hardware::JointControlMode::off);
    }

    void halt() {
      kinematics.set_joint_speed({ { hardware::Joint::left_wheel, 0 }, { hardware::Joint::right_wheel, 0 } });
    }

    void up(bool key_down) {
      if (key_down) {
        kinematics.set_joint_speed({ { hardware::Joint::left_wheel, -max_rpm }, { hardware::Joint::right_wheel, max_rpm } });
      } else {
        halt();
      }
    }

    void down(bool key_down) {
      if (key_down) {
        kinematics.set_joint_speed({ { hardware::Joint::left_wheel, max_rpm }, { hardware::Joint::right_wheel, -max_rpm } });
      } else {
        halt();
      }
    }

    void left(bool key_down) {
      if (key_down) {
        kinematics.set_joint_speed({ { hardware::Joint::left_wheel, max_rpm }, { hardware::Joint::right_wheel, max_rpm } });
      } else {
        halt();
      }
    }

    void right(bool key_down) {
      if (key_down) {
        kinematics.set_joint_speed({ { hardware::Joint::left_wheel, -max_rpm }, { hardware::Joint::right_wheel, -max_rpm } });
      } else {
        halt();
      }
    }

  protected:
    hardware::Kinematics& kinematics;

  private:
    const double max_rpm = 20;
    const double initial_ankle_angle = 360.0 / 16;
};

/**
 * Skiing is an automotive driving stunt where the car is driven while balanced
 * only on two wheels, either the pair on the driver side or on the passenger side.
 * See: https://en.wikipedia.org/wiki/Ski_(driving_stunt)
 */
class LocomotionSki: public Locomotion {
  public:
    LocomotionSki(hardware::Kinematics& kinematics) : kinematics(kinematics) {
    }

    std::string name() {
      return "Ski";
    }

    void start() {
      kinematics.set_joint_control_mode(hardware::Joint::left_wheel, hardware::JointControlMode::velocity);
      kinematics.set_joint_control_mode(hardware::Joint::left_ankle, hardware::JointControlMode::position);
      kinematics.set_joint_control_mode(hardware::Joint::right_ankle, hardware::JointControlMode::position);
      kinematics.set_joint_control_mode(hardware::Joint::right_wheel, hardware::JointControlMode::velocity);
    }

  protected:
    hardware::Kinematics& kinematics;
};

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
    }));
}

Robot::Robot(telemetry::Items& telemetryItems, IMU& imu, hardware::Kinematics& kinematics) :
    telemetryItems(telemetryItems), imu(imu), kinematics(kinematics) {
  model = new Model(telemetryItems);

  LocomotionIdle* locomotion_idle = new LocomotionIdle(kinematics);
  LocomotionTiltDrive* locomotion_tilt_drive = new LocomotionTiltDrive(kinematics);
  LocomotionSki* locomotion_ski = new LocomotionSki(kinematics);
  current_locomotion_mode = locomotion_idle;

  mode = new telemetry::ItemString(telemetry::ROOT_ITEM_ID, "Mode of operation", current_locomotion_mode->name());
  telemetryItems.add_item(mode);

  add_locomotion(locomotion_idle, "Escape");
  add_locomotion(locomotion_tilt_drive, "Digit1");
  add_locomotion(locomotion_ski, "Digit2");

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
    });
  telemetryItems.add_item(up);

  telemetry::ItemCommand* down = new telemetry::ItemCommand(
    mode->getId(), "Down", "ArrowDown", [&](int value) {
      current_locomotion_mode->down(value == 1);
    });
  telemetryItems.add_item(down);

  telemetry::ItemCommand* left = new telemetry::ItemCommand(
    mode->getId(), "Left", "ArrowLeft", [&](int value) {
      current_locomotion_mode->left(value == 1);
    });
  telemetryItems.add_item(left);

  telemetry::ItemCommand* right = new telemetry::ItemCommand(
    mode->getId(), "Right", "ArrowRight", [&](int value) {
      current_locomotion_mode->right(value == 1);
    });
  telemetryItems.add_item(right);
}

Robot::~Robot() {
  delete model;
  // TODO: delete locomotion modes
}
