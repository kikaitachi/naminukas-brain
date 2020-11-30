#ifndef NAMINUKAS_BRAIN_LOCOMOTION_IDLE_H_
#define NAMINUKAS_BRAIN_LOCOMOTION_IDLE_H_

#include "Locomotion.hpp"

class LocomotionIdle: public Locomotion {
  public:
    LocomotionIdle(hardware::Kinematics& kinematics);
    std::string name();
    void start();

  protected:
    hardware::Kinematics& kinematics;
};

#endif // NAMINUKAS_BRAIN_LOCOMOTION_IDLE_H_
