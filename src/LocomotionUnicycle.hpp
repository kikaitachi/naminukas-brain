#ifndef NAMINUKAS_BRAIN_LOCOMOTION_UNICYCLE_H_
#define NAMINUKAS_BRAIN_LOCOMOTION_UNICYCLE_H_

#include <thread>

#include "IMU.hpp"
#include "Locomotion.hpp"

/**
 * Attempt to drive on a single wheel.
 */
class LocomotionUnicycle: public Locomotion {
  public:
    LocomotionUnicycle(hardware::Kinematics& kinematics, IMU& imu);
    std::string name();
    void start();
    void stop();
    void up(bool key_down);
    void down(bool key_down);

  protected:
    hardware::Kinematics& kinematics;
    IMU& imu;
    std::thread* odometry_thread;
    bool stopped = true;
};

#endif // NAMINUKAS_BRAIN_LOCOMOTION_UNICYCLE_H_
