#ifndef NAMINUKAS_BRAIN_LOCOMOTION_SEGWAY_H_
#define NAMINUKAS_BRAIN_LOCOMOTION_SEGWAY_H_

#include "IMU.hpp"
#include "Locomotion.hpp"

/**
 * Segway like balancing on two wheels.
 */
class LocomotionSegway: public Locomotion {
  public:
    LocomotionSegway(hardware::Kinematics& kinematics, IMU& imu);
    std::string name();
    void control_loop();
    void on_start();
    void on_stop();
    void up(bool key_down);
    void down(bool key_down);

  protected:
    hardware::Kinematics& kinematics;
    IMU& imu;
    const float max_bias = 1;
    const float expected_pitch = 0;
    float prev_error;
    std::vector<hardware::JointPosition> expected_pos;
};

#endif // NAMINUKAS_BRAIN_LOCOMOTION_SEGWAY_H_
