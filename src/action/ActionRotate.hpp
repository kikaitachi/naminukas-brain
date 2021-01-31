#ifndef NAMINUKAS_BRAIN_ACTION_ROTATE_H_
#define NAMINUKAS_BRAIN_ACTION_ROTATE_H_

#include "../Hardware.hpp"
#include "Action.hpp"

class ActionRotate : public Action {
  public:
    ActionRotate(hardware::Kinematics& kinematics, std::vector<hardware::JointPosition> positions, int how);
    void start();
    void abort();
    bool execute();

    static const int ABSOLUTE = 0;
    static const int RELATIVE = 1;

  private:
    hardware::Kinematics& kinematics;
    std::vector<hardware::JointPosition> positions, goal;
    int how;

    std::vector<hardware::Joint> joints(std::vector<hardware::JointPosition> positions);
};

#endif  // NAMINUKAS_BRAIN_ACTION_ROTATE_H_
