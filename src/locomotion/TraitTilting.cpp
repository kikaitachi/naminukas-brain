#include "TraitTilting.hpp"

TraitTilting::TraitTilting(hardware::Kinematics& kinematics) : kinematics(kinematics) {
}

void TraitTilting::tilt() {
  kinematics.set_joint_position({
    { hardware::Joint::left_ankle, flat_ankle_angle + tilt_angle },
    { hardware::Joint::right_ankle, flat_ankle_angle - tilt_angle },
  });
}
