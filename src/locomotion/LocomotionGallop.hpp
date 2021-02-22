#ifndef SRC_LOCOMOTION_LOCOMOTIONGALLOP_HPP_
#define SRC_LOCOMOTION_LOCOMOTIONGALLOP_HPP_

#include <string>
#include <vector>

#include "Locomotion.hpp"

/**
 * Attempt to perform galloping motion by periodically tilting both wheels in
 * the same direction and tilting back to stationaty position.
 *
 * Doesn't work yet. Servo motors currently in use might be just too slow for
 * such locomotion. Potentially more speed could be gained tilting one wheel by
 * 90 degress instead of 45 or both wheels by 90 by changing mechanical design.
 */
class LocomotionGallop: public Locomotion {
 public:
  explicit LocomotionGallop(hardware::Kinematics& kinematics);
  std::string name();
  Pose control_loop(Pose pose);
  void on_start();
  void on_stop();
 private:
  hardware::Kinematics& kinematics;
  std::vector<hardware::JointPosition> initial_pos;
  bool going_up;
};

#endif  // SRC_LOCOMOTION_LOCOMOTIONGALLOP_HPP_
