#ifndef NAMINUKAS_BRAIN_LOCOMOTION_CATERPILLAR_H_
#define NAMINUKAS_BRAIN_LOCOMOTION_CATERPILLAR_H_

#include "Locomotion.hpp"

/**
 * Caterpillar like motion. In this mode of locomotion joints move
 * periodicallly without ever making full rotation which allows
 * robot to move even if placed in a bag.
 */
class LocomotionCaterpillar: public Locomotion {
  public:
    LocomotionCaterpillar(hardware::Kinematics& kinematics);
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
};

#endif // NAMINUKAS_BRAIN_CATERPILLAR_SKI_H_
