#include "../Logger.hpp"
#include "ActionRotate.hpp"

ActionRotate::ActionRotate(hardware::Kinematics& kinematics, std::vector<hardware::JointPosition> positions, int how)
    : kinematics(kinematics), positions(positions), how(how) {
}

void ActionRotate::start() {
  std::vector<hardware::JointPosition> destination;
  if (how == ABSOLUTE) {
    destination = positions;
  } else {
    destination = kinematics.get_joint_position(joints(positions));
    if (destination.size() == positions.size()) {
      for (int i = 0; i < positions.size(); i++) {
        destination[i].degrees += positions[i].degrees;
      }
    } else {
      logger::error("Failed to retrieve current joint angles for relative rotation action");
    }
  }
  kinematics.set_joint_position(destination);
}

void ActionRotate::abort() {
  // TODO: stop at current position
}

bool ActionRotate::execute() {
  return kinematics.reached_destination(joints(positions));
}

std::vector<hardware::Joint> ActionRotate::joints(std::vector<hardware::JointPosition> positions) {
  std::vector<hardware::Joint> joints;
  for (auto& position : positions) {
    joints.push_back(position.joint);
  }
  return joints;
}
