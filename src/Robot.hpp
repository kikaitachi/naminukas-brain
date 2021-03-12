#ifndef SRC_ROBOT_HPP_
#define SRC_ROBOT_HPP_

#include <list>
#include <memory>
#include <string>

#include "Camera.hpp"
#include "Hardware.hpp"
#include "locomotion/Locomotion.hpp"
#include "Model.hpp"
#include "Telemetry.hpp"

class Robot {
 public:
  Robot(
    telemetry::Items& telemetryItems,
    hardware::IMU& imu,
    hardware::Kinematics& kinematics,
    hardware::Pneumatics& pneumatics,
    Model& model,
    PointCloud& camera);
  ~Robot();

 private:
  hardware::IMU& imu;
  hardware::Kinematics& kinematics;
  hardware::Pneumatics& pneumatics;
  telemetry::Items& telemetryItems;
  std::shared_ptr<telemetry::ItemString> mode;
  std::list<Locomotion*> locomotion_modes;
  Locomotion* current_locomotion_mode;
  Model& model;
  bool pump_state = false;

  void add_locomotion(Locomotion* locomotion, std::string key);
  void play();
};

#endif  // SRC_ROBOT_HPP_
