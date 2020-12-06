#include <cmath>
#include <limits>

#include "Logger.hpp"
#include "LocomotionPoleGestures.hpp"

#define MAX_RPM 30

LocomotionPoleGestures::LocomotionPoleGestures(hardware::Kinematics& kinematics, Model& model, PointCloud& camera)
    : LocomotionPole(kinematics, model) {
  camera.add_depth_listener([&](rs2::depth_frame& depth_frame) {
    int width = depth_frame.get_width();
    int heigth = depth_frame.get_height();
    float min_left = std::numeric_limits<float>::max();
    float min_right = std::numeric_limits<float>::max();
    int min_left_index, min_right_index;
    for (int i = 0; i < width; i++) {
      float distance = depth_frame.get_distance(i, heigth / 2);
      if (distance > 0 && distance < 0.3) {
        if (i < width / 2) {
          if (distance < min_left) {
            min_left = distance;
            min_left_index = width / 2 - i - 1;
          }
        } else {
          if (distance < min_right) {
            min_right = distance;
            min_right_index = i - width / 2;
          }
        }
      }
    }
    if (!stopped) {
      logger::debug("Min left: %f, min right: %f", min_left, min_right);
      if (min_left == std::numeric_limits<float>::max() && min_right == std::numeric_limits<float>::max()) {
        halt();
      } else if (min_left == std::numeric_limits<float>::max()) {
        float rpm = MAX_RPM * min_right_index / (width / 2);
        kinematics.set_joint_speed({ { hardware::Joint::left_wheel, rpm }, { hardware::Joint::right_wheel, -rpm } });
      } else if (min_right == std::numeric_limits<float>::max()) {
        float rpm = MAX_RPM * min_left_index / (width / 2);
        kinematics.set_joint_speed({ { hardware::Joint::left_wheel, -rpm }, { hardware::Joint::right_wheel, rpm } });
      } else {
        float rpm = MAX_RPM * (fabs(min_left - min_right)) / 0.15;
        if (min_left < min_right) {
          kinematics.set_joint_speed({ { hardware::Joint::left_wheel, rpm }, { hardware::Joint::right_wheel, rpm } });
        } else {
          kinematics.set_joint_speed({ { hardware::Joint::left_wheel, -rpm }, { hardware::Joint::right_wheel, -rpm } });
        }
      }
    }
  });
}

std::string LocomotionPoleGestures::name() {
  return "Pole with gesture control";
}
