#include <cmath>
#include <limits>
#include <set>
#include <string>

#include "LocomotionSegway.hpp"

#define CONTROL_LOOP_FREQUENCY 100
#define ANKLE_DURATION_MS 500
#define SIDESTEP_MAGNITUDE 10

#define LOW_PASS(prev, new, alpha) prev + alpha * (new - prev)

LocomotionSegway::LocomotionSegway(hardware::Kinematics& kinematics, hardware::IMU& imu)
    : Locomotion(CONTROL_LOOP_FREQUENCY), TraitTilting(kinematics), kinematics(kinematics), imu(imu),
    speed_controller(0.2, 0, 0, 25, -40, 40),
    pitch_controller(2.5, 0, 0, 25, -std::numeric_limits<double>::max(), std::numeric_limits<double>::max()) {
}

std::string LocomotionSegway::name() {
  return "Segway";
}

Pose LocomotionSegway::control_loop(Pose pose) {
  struct timespec start_time;
  struct timespec end_time;
  clock_gettime(CLOCK_MONOTONIC, &start_time);
  std::vector<hardware::JointState> curr_pos = kinematics.get_joint_state({
    hardware::Joint::left_wheel,
    hardware::Joint::right_wheel
  });
  clock_gettime(CLOCK_MONOTONIC, &end_time);
  if (curr_pos.size() != 2) {
    return pose;
  }

  double rpm_left = curr_pos[0].rpm;
  double rpm_right = -curr_pos[1].rpm;
  rpm_avg = LOW_PASS(rpm_avg, (rpm_left + rpm_right) / 2, 0.05);
  // rpm_avg = (rpm_left + rpm_right) / 2;
  // double goal_pitch = -2.49;
  double goal_pitch = -2.49 - speed_controller.input(rpm_avg, goal_rpm);
  // pitch = LOW_PASS(pitch, imu.get_pitch(), 0.5);
  pitch = imu.get_pitch();
  double input = pitch_controller.input(pitch, goal_pitch);

  double new_position_left = curr_pos[0].position + input + left_turn_speed;
  double new_position_right = curr_pos[1].position - input - right_turn_speed;
  std::vector<hardware::JointPosition> positions;
  if (curr_pos[0].position != new_position_left) {
    positions.push_back({ hardware::Joint::left_wheel, new_position_left });
  }
  if (curr_pos[1].position != new_position_right) {
    positions.push_back({ hardware::Joint::right_wheel, new_position_right });
  }
  kinematics.set_joint_position(positions);

  if (control_loop_iteration % 50 == 0) {
    if (sidestep_direction_left != 0) {
      kinematics.set_joint_position({
        { hardware::Joint::left_ankle, initial_ankle_angle - tilt_angle +
            SIDESTEP_MAGNITUDE * sidestep_direction_left },
      });
      sidestep_direction_left = -sidestep_direction_left;
    }
  } else if ((control_loop_iteration + 25) % 50 == 0) {
    if (sidestep_direction_right != 0) {
      kinematics.set_joint_position({
        { hardware::Joint::right_ankle, initial_ankle_angle + tilt_angle +
            SIDESTEP_MAGNITUDE * sidestep_direction_right },
      });
      sidestep_direction_right = -sidestep_direction_right;
    }
  }

  logger::info("pitch: %f, speed / pitch fitness: %f / %f, rpm: %f / %f, read: %dns",
    imu.get_pitch(), speed_controller.get_fitness(), pitch_controller.get_fitness(),
    rpm_left, rpm_right, static_cast<int>((end_time.tv_sec - start_time.tv_sec) *
      1000000000 + (end_time.tv_nsec - start_time.tv_nsec)));

  control_loop_iteration++;

  // TODO: calculate new pose
  return pose;
}

void LocomotionSegway::on_start() {
  kinematics.set_joint_control_mode(hardware::Joint::left_wheel, hardware::JointControlMode::position);
  kinematics.set_joint_control_mode(hardware::Joint::left_ankle, hardware::JointControlMode::time,
    0, 0, ANKLE_DURATION_MS / 2, ANKLE_DURATION_MS);
  kinematics.set_joint_control_mode(hardware::Joint::right_ankle, hardware::JointControlMode::time,
    0, 0, ANKLE_DURATION_MS / 2, ANKLE_DURATION_MS);
  kinematics.set_joint_control_mode(hardware::Joint::right_wheel, hardware::JointControlMode::position);
  tilt_angle = -90;
  tilt();
  speed_controller.reset();
  pitch_controller.reset();
  pos_speed = left_turn_speed = right_turn_speed = 0;
  goal_rpm = rpm_avg = 0;
  pitch = imu.get_pitch();
  sidestep_direction_left = sidestep_direction_right = 0;
  control_loop_iteration = 0;
}

void LocomotionSegway::on_stop() {
  // Power down motors
  kinematics.set_joint_control_mode(hardware::Joint::left_wheel, hardware::JointControlMode::off);
  kinematics.set_joint_control_mode(hardware::Joint::left_ankle, hardware::JointControlMode::off);
  kinematics.set_joint_control_mode(hardware::Joint::right_ankle, hardware::JointControlMode::off);
  kinematics.set_joint_control_mode(hardware::Joint::right_wheel, hardware::JointControlMode::off);
}

void LocomotionSegway::stop() {
  Locomotion::stop();
  /*kinematics.set_joint_control_mode(hardware::Joint::left_wheel, hardware::JointControlMode::time, 0, 0, 0, 100);
  kinematics.set_joint_control_mode(hardware::Joint::left_ankle, hardware::JointControlMode::time, 0, 0, 0, 100);
  kinematics.set_joint_control_mode(hardware::Joint::right_ankle, hardware::JointControlMode::time, 0, 0, 0, 100);
  kinematics.set_joint_control_mode(hardware::Joint::right_wheel, hardware::JointControlMode::time, 0, 0, 0, 100);
  std::vector<hardware::JointPosition> curr_pos = kinematics.get_joint_position({
    hardware::Joint::left_wheel,
    hardware::Joint::left_ankle,
    hardware::Joint::right_ankle,
    hardware::Joint::right_wheel
  });
  kinematics.set_joint_position({
    { hardware::Joint::left_wheel, curr_pos[0].degrees - (curr_pos[1].degrees - initial_ankle_angle) },
    { hardware::Joint::left_ankle, initial_ankle_angle + 5 },
    { hardware::Joint::right_ankle, initial_ankle_angle - 5 },
    { hardware::Joint::right_wheel, curr_pos[3].degrees - (curr_pos[2].degrees - initial_ankle_angle) }
  });
  for ( ; ; ) {
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    std::vector<hardware::JointPosition> position = kinematics.get_joint_position({
      hardware::Joint::left_ankle,
      hardware::Joint::right_ankle
    });
    if (abs(position[0].degrees - initial_ankle_angle - 5) < 2 && abs(position[1].degrees - initial_ankle_angle + 5) < 2) {
      break;
    }
  };
  kinematics.set_joint_control_mode(hardware::Joint::left_wheel, hardware::JointControlMode::off);
  kinematics.set_joint_control_mode(hardware::Joint::left_ankle, hardware::JointControlMode::off);
  kinematics.set_joint_control_mode(hardware::Joint::right_ankle, hardware::JointControlMode::off);
  kinematics.set_joint_control_mode(hardware::Joint::right_wheel, hardware::JointControlMode::off);*/
}

void LocomotionSegway::up(bool key_down, std::set<std::string>& modifiers) {
  if (key_down) {
    if (modifiers.find("Control") != modifiers.end()) {
      /*kinematics.set_joint_position({
        { hardware::Joint::left_ankle, initial_ankle_angle - 80 },
        { hardware::Joint::right_ankle, initial_ankle_angle + 100 },
      });*/
      // sidestep_direction_left = 1;
      // sidestep_direction_right = 1;
      tilt_angle += 5;
      tilt();
    } else {
      pos_speed = 2;
      goal_rpm = 10;
    }
  } else {
    pos_speed = 0;
    goal_rpm = 0;
    sidestep_direction_left = 0;
    sidestep_direction_right = 0;
  }
}

void LocomotionSegway::down(bool key_down, std::set<std::string>& modifiers) {
  if (key_down) {
    if (modifiers.find("Control") != modifiers.end()) {
      /*kinematics.set_joint_position({
        { hardware::Joint::left_ankle, initial_ankle_angle - 80 },
        { hardware::Joint::right_ankle, initial_ankle_angle + 100 },
      });*/
      // sidestep_direction_left = 1;
      // sidestep_direction_right = -1;
      tilt_angle -= 5;
      tilt();
    } else {
      pos_speed = -2;
      goal_rpm = -10;
    }
  } else {
    pos_speed = 0;
    goal_rpm = 0;
    sidestep_direction_left = 0;
    sidestep_direction_right = 0;
  }
}

void LocomotionSegway::left(bool key_down, std::set<std::string>& modifiers) {
  if (key_down) {
    if (modifiers.find("Control") != modifiers.end()) {
      left_turn_speed = 0;
      right_turn_speed = -6;
    } else {
      left_turn_speed = 3;
      right_turn_speed = -3;
    }
  } else {
    left_turn_speed = right_turn_speed = 0;
  }
}

void LocomotionSegway::right(bool key_down, std::set<std::string>& modifiers) {
  if (key_down) {
    if (modifiers.find("Control") != modifiers.end()) {
      left_turn_speed = -6;
      right_turn_speed = 0;
    } else {
      left_turn_speed = -3;
      right_turn_speed = 3;
    }
  } else {
    left_turn_speed = right_turn_speed = 0;
  }
}
