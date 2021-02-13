#ifndef NAMINUKAS_BRAIN_MODEL_H_
#define NAMINUKAS_BRAIN_MODEL_H_

#include <vector>

#include "Hardware.hpp"
#include "Telemetry.hpp"

class Model {
  public:
    Model(telemetry::Items& telemetryItems, hardware::Kinematics& kinematics);

    /**
     * Set robot location and rotation of parts to the initial position.
     */
    void reset();

    void move(double delta_x, double delta_y, double delta_z);
    void rotate(double delta_x, double delta_y, double delta_z);

    void update_joints(std::vector<hardware::JointPosition> joints);

  private:
    std::shared_ptr<telemetry::Item3DModel> left_foot;
    std::shared_ptr<telemetry::Item3DModel> right_foot;
    double pos_x, pos_y, pos_z;
    double rot_x, rot_y, rot_z;
    double left_wheel_pos, right_wheel_pos;

    std::vector<Transform> left_foot_transforms();
    std::vector<Transform> right_foot_transforms();
};

#endif  // NAMINUKAS_BRAIN_MODEL_H_
