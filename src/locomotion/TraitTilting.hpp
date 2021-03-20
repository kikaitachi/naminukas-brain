#ifndef SRC_LOCOMOTION_TRAITTILTING_HPP_
#define SRC_LOCOMOTION_TRAITTILTING_HPP_

#include "../Hardware.hpp"

class TraitTilting {
 public:
  explicit TraitTilting(hardware::Kinematics& kinematics);
 protected:
  hardware::Kinematics& kinematics;
  const double flat_ankle_angle = 360.0 / 16;
  double tilt_angle;

  void tilt();
};

#endif  // SRC_LOCOMOTION_TRAITTILTING_HPP_
