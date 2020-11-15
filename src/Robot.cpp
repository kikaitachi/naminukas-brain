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
    }

    void stop() {
      kinematics.set_joint_control_mode(hardware::Joint::left_wheel, hardware::JointControlMode::off);
      kinematics.set_joint_control_mode(hardware::Joint::left_ankle, hardware::JointControlMode::off);
      kinematics.set_joint_control_mode(hardware::Joint::right_ankle, hardware::JointControlMode::off);
      kinematics.set_joint_control_mode(hardware::Joint::right_wheel, hardware::JointControlMode::off);
    }

    void up() {
      kinematics.set_joint_speed({ { hardware::Joint::left_wheel, 2.5 }, { hardware::Joint::right_wheel, 2.5 } });
    }

    void down() {
      kinematics.set_joint_speed({ { hardware::Joint::left_wheel, 0 }, { hardware::Joint::right_wheel, 0 } });
    }

    void left() {
      kinematics.set_joint_speed({ { hardware::Joint::left_wheel, 2.5 }, { hardware::Joint::right_wheel, -2.5 } });
    }

    void right() {
      kinematics.set_joint_speed({ { hardware::Joint::left_wheel, -2.5 }, { hardware::Joint::right_wheel, 2.5 } });
    }

  protected:
    hardware::Kinematics& kinematics;
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

std::mutex Robot::stateMutex;

void Robot::add_locomotion(Locomotion* locomotion, std::string key) {
  telemetryItems.add_item(new telemetry::ItemCommand(
    mode->getId(), locomotion->name(), key, [=]() {
      logger::debug("Stopping locomotion: %s", current_locomotion_mode->name().c_str());
      current_locomotion_mode->stop();
      current_locomotion_mode = locomotion;
      logger::debug("Starting locomotion: %s", current_locomotion_mode->name().c_str());
      current_locomotion_mode->start();
      mode->update(current_locomotion_mode->name());
    }));
}

Robot::Robot(telemetry::Items& telemetryItems, IMU& imu, hardware::Kinematics& kinematics) :
    telemetryItems(telemetryItems), imu(imu), kinematics(kinematics) {
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
    mode->getId(), "Up", "ArrowUp", [&]() {
      current_locomotion_mode->up();
    });
  telemetryItems.add_item(up);

  telemetry::ItemCommand* down = new telemetry::ItemCommand(
    mode->getId(), "Down", "ArrowDown", [&]() {
      current_locomotion_mode->down();
    });
  telemetryItems.add_item(down);

  telemetry::ItemCommand* left = new telemetry::ItemCommand(
    mode->getId(), "Left", "ArrowLeft", [&]() {
      current_locomotion_mode->left();
    });
  telemetryItems.add_item(left);

  telemetry::ItemCommand* right = new telemetry::ItemCommand(
    mode->getId(), "Right", "ArrowRight", [&]() {
      current_locomotion_mode->right();
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
