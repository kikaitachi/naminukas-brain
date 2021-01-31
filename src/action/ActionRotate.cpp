#include <cmath>
#include "../Logger.hpp"
#include "ActionRotate.hpp"

#define POS_THRESHOLD 0.2

ActionRotate::ActionRotate(hardware::Kinematics& kinematics, std::vector<hardware::JointPosition> positions, int how)
    : kinematics(kinematics), positions(positions), how(how) {
}

void ActionRotate::start() {
  if (how == ABSOLUTE) {
    goal = positions;
  } else {
    goal = kinematics.get_joint_position(joints(positions));
    if (goal.size() == positions.size()) {
      for (int i = 0; i < positions.size(); i++) {
        goal[i].degrees += positions[i].degrees;
      }
    } else {
      logger::error("Failed to retrieve current joint angles for relative rotation action");
    }
  }
  kinematics.set_joint_position(goal);
}

void ActionRotate::abort() {
  // TODO: stop at current position
}

bool ActionRotate::execute() {
  std::vector<hardware::JointPosition> current = kinematics.get_joint_position(joints(positions));
  for (int i = 0; i < current.size(); i++) {
    if (fabs(current[i].degrees - goal[i].degrees) > POS_THRESHOLD) {
      return false;
    }
  }
  return true;
}

std::vector<hardware::Joint> ActionRotate::joints(std::vector<hardware::JointPosition> positions) {
  std::vector<hardware::Joint> joints;
  for (auto& position : positions) {
    joints.push_back(position.joint);
  }
  return joints;
}
