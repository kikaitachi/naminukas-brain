#ifndef SRC_LOCOMOTION_LOCOMOTIONWADDLE_HPP_
#define SRC_LOCOMOTION_LOCOMOTIONWADDLE_HPP_

#include "Locomotion.hpp"

/**
 * Waddle like motion. In this mode of locomotion joints move
 * periodicallly without ever making full rotation which allows
 * robot to move even if placed in a bag.
 */
class LocomotionWaddle: public Locomotion {
 public:
  explicit LocomotionWaddle(hardware::Kinematics& kinematics);
  std::string name();
  Pose control_loop(Pose pose);
  void on_start();
  void on_stop();
  void up(bool key_down, std::set<std::string>& modifiers);
  void down(bool key_down, std::set<std::string>& modifiers);
  void left(bool key_down, std::set<std::string>& modifiers);
  void right(bool key_down, std::set<std::string>& modifiers);

 private:
  hardware::Kinematics& kinematics;
  int control_loop_iteration;
  int tilt_direction;
  int drive_direction;
  double left_drive_distance, right_drive_distance;
  std::vector<hardware::JointPosition> initial_pos;
};

#endif  // SRC_LOCOMOTION_LOCOMOTIONWADDLE_HPP_
