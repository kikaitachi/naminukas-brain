#ifndef SRC_LOCOMOTION_LOCOMOTIONHANG_HPP_
#define SRC_LOCOMOTION_LOCOMOTIONHANG_HPP_

#include <set>
#include <string>
#include <vector>

#include "Locomotion.hpp"

/**
 * Drive while hanging on one wheel.
 */
class LocomotionHang: public Locomotion {
 public:
  explicit LocomotionHang(hardware::Kinematics& kinematics);
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
  const double max_rpm = 5;
  void halt();
};

#endif  // SRC_LOCOMOTION_LOCOMOTIONHANG_HPP_
