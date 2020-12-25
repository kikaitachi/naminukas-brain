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
    void up(bool key_down, std::set<std::string>& modifiers);
    void down(bool key_down, std::set<std::string>& modifiers);

  protected:
    hardware::Kinematics& kinematics;
    IMU& imu;
    float prev_rpm;
    float goal_rpm;
    std::vector<hardware::JointPosition> expected_pos;
    std::vector<hardware::JointPosition> prev_pos;
};

#endif // NAMINUKAS_BRAIN_LOCOMOTION_UNICYCLE_H_
