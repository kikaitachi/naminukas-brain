#ifndef NAMINUKAS_BRAIN_CAMERA_H_
#define NAMINUKAS_BRAIN_CAMERA_H_

#include <functional>
#include <vector>

#include "Telemetry.hpp"
#include <librealsense2/rs.hpp>

class PointCloud {
  public:
    PointCloud(telemetry::Items& telemetryItems, std::function<bool()> is_terminated);
    void add_depth_listener(std::function<void(rs2::depth_frame&)> listener);
    void remove_depth_listener(std::function<void(rs2::depth_frame&)> listener);

  private:
    telemetry::ItemPoints* points_telemetry;
    std::vector<std::function<void(rs2::depth_frame&)>> depth_listeners;
};

#endif  // NAMINUKAS_BRAIN_CAMERA_H_
