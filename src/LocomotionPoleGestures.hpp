#ifndef NAMINUKAS_BRAIN_LOCOMOTION_POLE_GESTURES_H_
#define NAMINUKAS_BRAIN_LOCOMOTION_POLE_GESTURES_H_

#include "Camera.hpp"
#include "LocomotionPole.hpp"

class LocomotionPoleGestures: public LocomotionPole {
  public:
    LocomotionPoleGestures(hardware::Kinematics& kinematics, Model& model, PointCloud& camera);
    std::string name();
    void start();
    void stop();

  private:
};

#endif // NAMINUKAS_BRAIN_LOCOMOTION_POLE_GESTURES_H_
