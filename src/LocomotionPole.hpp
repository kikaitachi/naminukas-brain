#ifndef NAMINUKAS_BRAIN_LOCOMOTION_POLE_H_
#define NAMINUKAS_BRAIN_LOCOMOTION_POLE_H_

#include <thread>

#include "Locomotion.hpp"
#include "Model.hpp"

class LocomotionPole: public Locomotion {
  public:
    LocomotionPole(hardware::Kinematics& kinematics, Model& model);
    std::string name();
    void start();
    void stop();
    void up(bool key_down);
    void down(bool key_down);
    void left(bool key_down);
    void right(bool key_down);

  protected:
    hardware::Kinematics& kinematics;
    Model& model;
    std::thread* odometry_thread;
    bool stopped = true;
    const double max_rpm = 20;
    const double initial_ankle_angle = 360.0 / 16;

    void halt();
};

#endif // NAMINUKAS_BRAIN_LOCOMOTION_POLE_H_
