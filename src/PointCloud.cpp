#include <thread>
#include <librealsense2/rs.hpp>

#include "Logger.hpp"
#include "PointCloud.hpp"

PointCloud::PointCloud(std::function<bool()> is_terminated) {
  std::thread video_thread([=]() {
    rs2::pointcloud pc;
    rs2::pipeline pipe;
    pipe.start();
    while (!is_terminated()) {
      auto frames = pipe.wait_for_frames();
      auto depth = frames.get_depth_frame();
      rs2::points points = pc.calculate(depth);
      logger::debug("Got %d points", points.size());
    }
  });
  video_thread.detach();
}
