#include "LocomotionPoleGestures.hpp"

LocomotionPoleGestures::LocomotionPoleGestures(hardware::Kinematics& kinematics, Model& model, PointCloud& camera)
    : LocomotionPole(kinematics, model) {
  camera.add_depth_listener([](rs2::depth_frame& depth_frame) {
    int width = depth_frame.get_width();
    int heigth = depth_frame.get_height();
    for (int i = 0; i < width; i++) {
      float distance = depth_frame.get_distance(i, heigth / 2);
    }
    // TODO: find min (< 1m) left, and min right
  });
}

std::string LocomotionPoleGestures::name() {
  return "Pole with gesture control";
}

void LocomotionPoleGestures::start() {
  //
}

void LocomotionPoleGestures::stop() {
  //
}
