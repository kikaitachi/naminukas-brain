#ifndef SRC_ROBOT_HPP_
#define SRC_ROBOT_HPP_

#include <functional>
#include <list>
#include <memory>
#include <string>

#include "Camera.hpp"
#include "Hardware.hpp"
#include "locomotion/Locomotion.hpp"
#include "Model.hpp"
#include "RCRadio.hpp"
#include "Telemetry.hpp"

enum class RCChannelState {
  low,
  middle,
  high
};

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
    RCRadio& rc_radio,
    std::function<bool()> is_terminated);
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
  std::function<bool()> is_terminated;

  /**
   * Ankle angle in degrees when feet are flat, i.e. not tiled.
   */
  const double flat_ankle_angle = 360.0 / 16;

  /**
   * How much ankle can tilt to outside relative to flat_ankle_angle.
   * -90 corresponds to robot being in Segway mode.
   */
  const double min_tilt_angle = -75;

  /**
   * How much ankle can tilt to inside relative to flat_ankle_angle.
   */
  const double max_tilt_angle = 35;

  bool pump_state = false;

  RCChannelState left_motor_channel = RCChannelState::high;
  RCChannelState right_motor_channel = RCChannelState::high;
  RCChannelState left_pump_channel = RCChannelState::high;
  RCChannelState right_pump_channel = RCChannelState::high;

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

  /**
   * Move depending on which state robot is.
   * Might be moving whole robot or just a single joint.
   *
   * @param speed Value from -1 to 1 proportional to desired moving speed.
   *              -1 means move forward at full speed.
   *              0 means stop.
   *              1 means move backward at full speed.
   */
  void move(double speed);

  void control_loop();
};

#endif  // SRC_ROBOT_HPP_
