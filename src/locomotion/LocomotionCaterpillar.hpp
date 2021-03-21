#ifndef SRC_LOCOMOTION_LOCOMOTIONCATERPILLAR_HPP_
#define SRC_LOCOMOTION_LOCOMOTIONCATERPILLAR_HPP_

#include <string>
#include <vector>

#include "Locomotion.hpp"

/**
 * Caterpillar like motion, where wheels are periodically tilting out of phase
 * with each other.
 */
class LocomotionCaterpillar: public Locomotion {
 public:
  explicit LocomotionCaterpillar(hardware::Kinematics& kinematics);
  std::string name();
  Pose control_loop(Pose pose);
  void on_start();
  void on_stop();
  void up(bool key_down, std::set<std::string>& modifiers);
  void down(bool key_down, std::set<std::string>& modifiers);
 private:
  hardware::Kinematics& kinematics;
  std::vector<hardware::JointPosition> initial_pos;
  int direction_left;
  int direction_right;
};

#endif  // SRC_LOCOMOTION_LOCOMOTIONCATERPILLAR_HPP_
