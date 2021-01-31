#include <cmath>
#include "../Logger.hpp"
#include "ActionRotate.hpp"

#define POS_THRESHOLD 2

ActionRotate::ActionRotate(hardware::Kinematics& kinematics, std::vector<JointRotation> rotations)
    : kinematics(kinematics), rotations(rotations) {
}

void ActionRotate::start() {
  goal = kinematics.get_joint_position(joints(rotations));
  if (goal.size() != rotations.size()) {
    logger::error("Failed to retrieve current joint angles for rotation action");
    return;
  }
  for (int i = 0; i < rotations.size(); i++) {
    if (rotations[i].type == RotationType::absolute) {
      goal[i].degrees = rotations[i].degrees;
    } else {
      goal[i].degrees += rotations[i].degrees;
    }
  }
  kinematics.set_joint_position(goal);
}

void ActionRotate::abort() {
  // TODO: stop at current position
}

bool ActionRotate::execute() {
  std::vector<hardware::JointPosition> current = kinematics.get_joint_position(joints(rotations));
  for (int i = 0; i < current.size(); i++) {
    if (fabs(current[i].degrees - goal[i].degrees) > POS_THRESHOLD) {
      return false;
    }
  }
  return true;
}

std::vector<hardware::Joint> ActionRotate::joints(std::vector<JointRotation> rotations) {
  std::vector<hardware::Joint> joints;
  for (auto& rotation : rotations) {
    joints.push_back(rotation.joint);
  }
  return joints;
}
