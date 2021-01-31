#ifndef NAMINUKAS_BRAIN_ACTION_ROTATE_H_
#define NAMINUKAS_BRAIN_ACTION_ROTATE_H_

#include "../Hardware.hpp"
#include "Action.hpp"

enum class RotationType {
  /** Go to given absolute angle */
  absolute,

  /** Go to current angle + given value */
  relative
};

class JointRotation {
  public:
    hardware::Joint joint;
    RotationType type;
    double degrees;
};

class ActionRotate : public Action {
  public:
    ActionRotate(hardware::Kinematics& kinematics, std::vector<JointRotation> rotations);
    void start();
    void abort();
    bool execute();

  private:
    hardware::Kinematics& kinematics;
    std::vector<JointRotation> rotations;
    std::vector<hardware::JointPosition> goal;

    std::vector<hardware::Joint> joints(std::vector<JointRotation> rotations);
};

#endif  // NAMINUKAS_BRAIN_ACTION_ROTATE_H_
