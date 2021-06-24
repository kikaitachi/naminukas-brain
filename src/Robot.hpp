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

  /**
   * Called when RC radio channel changes.
   *
   * @param channel Zero based channel index.
   * @param new_value Value from RCRadio::channel_min_value to RCRadio::channel_max_value.
   */
  void on_rc_radio_channel_change(int channel, int new_value);

  /**
   * Turn depending on which state robot is.
   * Might be turning whole robot or just a single joint.
   *
   * @param speed Value from -1 to 1 proportional to desired turning speed.
   *              -1 means turn left or counterclockwise at full speed.
   *              0 means stop.
   *              1 means turn right or clockwise at full speed.
   */
  void turn(double speed);
};

#endif  // SRC_ROBOT_HPP_
