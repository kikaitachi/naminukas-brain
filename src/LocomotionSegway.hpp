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
    void left(bool key_down);
    void right(bool key_down);

  protected:
    hardware::Kinematics& kinematics;
    IMU& imu;
    float prev_rpm;
    float pos_speed;
    float turn_speed;
    std::vector<hardware::JointPosition> expected_pos;
    std::vector<hardware::JointPosition> prev_pos;
};

#endif // NAMINUKAS_BRAIN_LOCOMOTION_SEGWAY_H_
