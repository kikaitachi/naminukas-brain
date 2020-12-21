#ifndef NAMINUKAS_BRAIN_LOCOMOTION_UNICYCLE_H_
#define NAMINUKAS_BRAIN_LOCOMOTION_UNICYCLE_H_

#include "IMU.hpp"
#include "Locomotion.hpp"

/**
 * Attempt to drive on a single wheel.
 */
class LocomotionUnicycle: public Locomotion {
  public:
    LocomotionUnicycle(hardware::Kinematics& kinematics, IMU& imu);
    std::string name();
    void control_loop();
    void on_start();
    void on_stop();
    void up(bool key_down);
    void down(bool key_down);

  protected:
    hardware::Kinematics& kinematics;
    IMU& imu;
};

#endif // NAMINUKAS_BRAIN_LOCOMOTION_UNICYCLE_H_
