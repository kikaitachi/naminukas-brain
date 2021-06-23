#ifndef SRC_ROBOT_HPP_
#define SRC_ROBOT_HPP_

#include <list>
#include <memory>
#include <string>

#include "Camera.hpp"
#include "Hardware.hpp"
#include "locomotion/Locomotion.hpp"
#include "Model.hpp"
#include "RCRadio.hpp"
#include "Telemetry.hpp"

class Robot : public RCRadioChannelChangedHandler {
 public:
  Robot(
    telemetry::Items& telemetryItems,
    hardware::IMU& imu,
    hardware::Barometer& barometer,
    hardware::Kinematics& kinematics,
    hardware::Pneumatics& pneumatics,
    Model& model,
    PointCloud& camera,
    RCRadio& rc_radio);
  ~Robot();

 private:
  hardware::IMU& imu;
  hardware::Barometer& barometer;
  hardware::Kinematics& kinematics;
  hardware::Pneumatics& pneumatics;
  telemetry::Items& telemetryItems;
  std::shared_ptr<telemetry::ItemString> mode;
  std::list<Locomotion*> locomotion_modes;
  Locomotion* current_locomotion_mode;
  Model& model;
  RCRadio& rc_radio;
  bool pump_state = false;

  void add_locomotion(Locomotion* locomotion, std::string key);
  void play();

  void on_rc_radio_channel_change(int channel, int new_value);
};

#endif  // SRC_ROBOT_HPP_
