#ifndef NAMINUKAS_BRAIN_LOCOMOTION_WADDLE_H_
#define NAMINUKAS_BRAIN_LOCOMOTION_WADDLE_H_

#include "Locomotion.hpp"
#include "../action/ActionSequential.hpp"

/**
 * Waddle like motion. In this mode of locomotion joints move
 * periodicallly without ever making full rotation which allows
 * robot to move even if placed in a bag.
 */
class LocomotionWaddle: public Locomotion {
  public:
    LocomotionWaddle(hardware::Kinematics& kinematics);
    std::string name();
    void control_loop();
    void on_start();
    void on_stop();
    void up(bool key_down, std::set<std::string>& modifiers);
    void down(bool key_down, std::set<std::string>& modifiers);
    void left(bool key_down, std::set<std::string>& modifiers);
    void right(bool key_down, std::set<std::string>& modifiers);

  private:
    hardware::Kinematics& kinematics;
    ActionSequential forward;
};

#endif // NAMINUKAS_BRAIN_WADDLE_H_
