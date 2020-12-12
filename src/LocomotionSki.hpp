#ifndef NAMINUKAS_BRAIN_LOCOMOTION_SKI_H_
#define NAMINUKAS_BRAIN_LOCOMOTION_SKI_H_

#include <thread>

#include "IMU.hpp"
#include "Locomotion.hpp"

/**
 * Skiing is an automotive driving stunt where the car is driven while balanced
 * only on two wheels, either the pair on the driver side or on the passenger side.
 * See: https://en.wikipedia.org/wiki/Ski_(driving_stunt)
 */
class LocomotionSki: public Locomotion {
  public:
    LocomotionSki(hardware::Kinematics& kinematics, IMU& imu);
    std::string name();
    void start();
    void stop();
    void up(bool key_down);
    void down(bool key_down);
    void left(bool key_down);
    void right(bool key_down);

  protected:
    hardware::Kinematics& kinematics;
    IMU& imu;
    std::thread* odometry_thread;
    bool stopped = true;
    const float max_rpm = 20;
    const float turn_rpm = 5;
    const float initial_ankle_angle = 360.0 / 16;
    const float max_ankle_change = 20;
    const float expected_pitch = 25.7;
};

#endif // NAMINUKAS_BRAIN_LOCOMOTION_SKI_H_
