#ifndef SRC_LOCOMOTION_LOCOMOTIONHANG_HPP_
#define SRC_LOCOMOTION_LOCOMOTIONHANG_HPP_

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
 private:
  hardware::Kinematics& kinematics;
};

#endif  // SRC_LOCOMOTION_LOCOMOTIONHANG_HPP_
