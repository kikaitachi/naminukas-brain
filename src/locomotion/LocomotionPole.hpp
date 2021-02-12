#ifndef NAMINUKAS_BRAIN_LOCOMOTION_POLE_H_
#define NAMINUKAS_BRAIN_LOCOMOTION_POLE_H_

#include "Locomotion.hpp"
#include "../Model.hpp"

class LocomotionPole: public Locomotion {
  public:
    LocomotionPole(hardware::Kinematics& kinematics, Model& model);
    std::string name();
    Pose control_loop(Pose pose);
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

    // Determine experimentally
    const double wheel_radius = 0.045;
    const double distance_between_wheels = 0.213;

    std::vector<hardware::JointPosition> previous_angles;

    void halt();
};

#endif // NAMINUKAS_BRAIN_LOCOMOTION_POLE_H_
