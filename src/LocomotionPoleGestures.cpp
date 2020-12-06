#include <limits>

#include "Logger.hpp"
#include "LocomotionPoleGestures.hpp"

LocomotionPoleGestures::LocomotionPoleGestures(hardware::Kinematics& kinematics, Model& model, PointCloud& camera)
    : LocomotionPole(kinematics, model) {
  camera.add_depth_listener([&](rs2::depth_frame& depth_frame) {
    int width = depth_frame.get_width();
    int heigth = depth_frame.get_height();
    float min_left = std::numeric_limits<float>::max();
    float min_right = std::numeric_limits<float>::max();
    for (int i = 0; i < width; i++) {
      float distance = depth_frame.get_distance(i, heigth / 2);
      if (distance > 0 && distance < 0.3) {
        if (i < width / 2) {
          if (distance < min_left) {
            min_left = distance;
          }
        } else {
          if (distance < min_right) {
            min_right = distance;
          }
        }
      }
    }
    if (!stopped) {
      logger::debug("Min left: %f, min right: %f", min_left, min_right);
      if (min_left == std::numeric_limits<float>::max() && min_right == std::numeric_limits<float>::max()) {
        halt();
      } else if (min_left == std::numeric_limits<float>::max()) {
        up(true);
      } else if (min_right == std::numeric_limits<float>::max()) {
        down(true);
      } else {
        if (min_left < min_right) {
          left(true);
        } else {
          right(true);
        }
      }
    }
  });
}

std::string LocomotionPoleGestures::name() {
  return "Pole with gesture control";
}
