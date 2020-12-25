#ifndef NAMINUKAS_BRAIN_LOCOMOTION_SKI_H_
#define NAMINUKAS_BRAIN_LOCOMOTION_SKI_H_

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
    void control_loop();
    void on_start();
    void on_stop();
    void up(bool key_down, std::set<std::string> modifiers);
    void down(bool key_down, std::set<std::string> modifiers);
    void left(bool key_down, std::set<std::string> modifiers);
    void right(bool key_down, std::set<std::string> modifiers);

  protected:
    hardware::Kinematics& kinematics;
    IMU& imu;
    const float max_rpm = 20;
    const float turn_rpm = 10;
    const float initial_ankle_angle = 360.0 / 16;
    const float max_ankle_change = 20;
    const float expected_pitch = 25.7;
    float prev_error;
};

#endif // NAMINUKAS_BRAIN_LOCOMOTION_SKI_H_
