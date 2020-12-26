#ifndef NAMINUKAS_BRAIN_LOCOMOTION_POLE_H_
#define NAMINUKAS_BRAIN_LOCOMOTION_POLE_H_

#include "Locomotion.hpp"
#include "../Model.hpp"

class LocomotionPole: public Locomotion {
  public:
    LocomotionPole(hardware::Kinematics& kinematics, Model& model);
    std::string name();
    void control_loop();
    void on_start();
    void on_stop();
    void up(bool key_down, std::set<std::string>& modifiers);
    void down(bool key_down, std::set<std::string>& modifiers);
    void left(bool key_down, std::set<std::string>& modifiers);
    void right(bool key_down, std::set<std::string>& modifiers);

  protected:
    hardware::Kinematics& kinematics;
    Model& model;
    const double max_rpm = 20;

    void halt();
};

#endif // NAMINUKAS_BRAIN_LOCOMOTION_POLE_H_
