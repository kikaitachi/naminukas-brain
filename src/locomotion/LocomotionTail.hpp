#ifndef NAMINUKAS_BRAIN_LOCOMOTION_TAIL_H_
#define NAMINUKAS_BRAIN_LOCOMOTION_TAIL_H_

#include "../IMU.hpp"
#include "Locomotion.hpp"

/**
 * Using tail for self-righting and locomotion or rough terrain.
 */
class LocomotionTail: public Locomotion {
  public:
    LocomotionTail(hardware::Kinematics& kinematics, IMU& imu);
    std::string name();
    void on_start();
    void on_stop();
    void up(bool key_down, std::set<std::string>& modifiers);
    void down(bool key_down, std::set<std::string>& modifiers);
    void left(bool key_down, std::set<std::string>& modifiers);
    void right(bool key_down, std::set<std::string>& modifiers);

  protected:
    hardware::Kinematics& kinematics;
    IMU& imu;
    const double max_rpm = 20;

    void halt();
};

#endif // NAMINUKAS_BRAIN_LOCOMOTION_TAIL_H_
