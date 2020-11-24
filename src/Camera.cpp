#include <cmath>
#include <fstream>
#include <limits>
#include <string>
#include <thread>
#include "webp/encode.h"
#include <librealsense2/rs.hpp>

#include "Logger.hpp"
#include "Camera.hpp"

#define DEPTH_STREAM       RS2_STREAM_DEPTH
#define DEPTH_FORMAT       RS2_FORMAT_Z16
#define DEPTH_WIDTH        480
#define DEPTH_HEIGHT       270
#define DEPTH_FPS          6
#define DEPTH_STREAM_INDEX -1

#define RGB_STREAM       RS2_STREAM_COLOR
#define RGB_FORMAT       RS2_FORMAT_RGB8
#define RGB_WIDTH        424
#define RGB_HEIGHT       240
#define RGB_FPS          6
#define RGB_STREAM_INDEX -1

#define INFRARED_STREAM       RS2_STREAM_INFRARED
#define INFRARED_FORMAT       RS2_FORMAT_RGB8
#define INFRARED_WIDTH        480
#define INFRARED_HEIGHT       270
#define INFRARED_FPS          6
#define INFRARED_STREAM_INDEX -1

static void image2file(std::string file_name, const uint8_t* rgb, int width, int height, int stride) {
  uint8_t* output;
  size_t size = WebPEncodeRGB(rgb, width, height, stride, 75, &output);
  std::ofstream file(file_name, std::ios::out | std::ios::binary);
  file.write((const char *)output, size);
  WebPFree(output);
}

PointCloud::PointCloud(telemetry::Items& telemetryItems, std::function<bool()> is_terminated) {
  points_telemetry = new telemetry::ItemPoints(telemetry::ROOT_ITEM_ID, "Point cloud", {});
  telemetry::Item* machine = telemetryItems.add_item(points_telemetry);

  std::thread video_thread([=]() {
    rs2::context ctx;

    rs2::device_list device_list = ctx.query_devices();

    if (device_list.size() > 0) {
      rs2::device dev = device_list[0];

      logger::info("Using RealSense device 0: %s", dev.get_info(RS2_CAMERA_INFO_NAME));

      rs2::pipeline pipeline(ctx);

      rs2::config config;

      config.enable_stream(RGB_STREAM, RGB_STREAM_INDEX, RGB_WIDTH, RGB_HEIGHT, RGB_FORMAT, RGB_FPS);
      config.enable_stream(DEPTH_STREAM, DEPTH_STREAM_INDEX, DEPTH_WIDTH, DEPTH_HEIGHT, DEPTH_FORMAT, DEPTH_FPS);
      config.enable_stream(INFRARED_STREAM, INFRARED_STREAM_INDEX, INFRARED_WIDTH, INFRARED_HEIGHT, INFRARED_FORMAT, INFRARED_FPS);

      rs2::pointcloud pc;
      rs2::colorizer color_map;

      pipeline.start(config);

      while (!is_terminated()) {
        rs2::frameset frameset;
        if (pipeline.try_wait_for_frames(&frameset, 1000000)) {
          auto color_frame = frameset.get_color_frame();
          auto depth_frame = frameset.get_depth_frame();
          auto infrared_frame = frameset.get_infrared_frame();

          int bytes_per_pixel = color_frame.get_bytes_per_pixel(); // Get # of bytes per pixel
          int stride_in_bytes = color_frame.get_stride_in_bytes(); // Get line width in bytes
          const auto texture = reinterpret_cast<const uint8_t*>(color_frame.get_data());

          const auto infrared = reinterpret_cast<const uint8_t*>(infrared_frame.get_data());
          const auto colored_depth_frame = color_map.process(depth_frame);
          const auto colored_depth = reinterpret_cast<const uint8_t*>(colored_depth_frame.get_data());

          image2file("rgb.webp", texture, RGB_WIDTH, RGB_HEIGHT, stride_in_bytes);
          image2file("infrared.webp", infrared, INFRARED_WIDTH, INFRARED_HEIGHT, infrared_frame.get_stride_in_bytes());
          image2file("depth.webp", colored_depth, DEPTH_WIDTH, DEPTH_HEIGHT, DEPTH_WIDTH * 3);

          pc.map_to(color_frame);
          rs2::points points = pc.calculate(depth_frame);
          auto texture_coordinates = points.get_texture_coordinates();

          int count = 0;
          float
            min_x = std::numeric_limits<float>::max(),
            max_x = std::numeric_limits<float>::min(),
            min_y = std::numeric_limits<float>::max(),
            max_y = std::numeric_limits<float>::min(),
            min_z = std::numeric_limits<float>::max(),
            max_z = std::numeric_limits<float>::min();

          std::vector<telemetry::ColoredPoint> colored_points;

          for (int i = 0; i < points.size(); i++) {
            rs2::vertex v = points.get_vertices()[i];
            if (v.z > 0 && v.z < 1000) { // 0 - 10m
              auto texture_coordinate = texture_coordinates[i];

              int x_value = int(texture_coordinate.u * RGB_WIDTH  + .5f);
              if (x_value < 0 || x_value > RGB_WIDTH - 1) {
                continue;
              }
              int y_value = int(texture_coordinate.v * RGB_HEIGHT + .5f);
              if (y_value < 0 || y_value > RGB_HEIGHT - 1) {
                continue;
              }

              int bytes = x_value * bytes_per_pixel;
              int strides = y_value * stride_in_bytes;
              int index = bytes + strides;

              uint8_t r = texture[index];
              uint8_t g = texture[index + 1];
              uint8_t b = texture[index + 2];

              if (v.x < min_x) {
                min_x = v.x;
              } else if (v.x > max_x) {
                max_x = v.x;
              }
              if (v.y < min_y) {
                min_y = v.y;
              } else if (v.y > max_y) {
                max_y = v.y;
              }
              if (v.z < min_z) {
                min_z = v.z;
              } else if (v.z > max_z) {
                max_z = v.z;
              }

              count++;
              colored_points.push_back({v.x, v.y, v.z, r, g, b});
            }
          }

          points_telemetry->update(colored_points);

          logger::debug("Got color %dx%d, depth %dx%d and infrared %dx%d frames, %d (%d valid) points: %f, %f, %f, %f, %f, %f",
            color_frame.get_width(), color_frame.get_height(),
            depth_frame.get_width(), depth_frame.get_height(),
            infrared_frame.get_width(), infrared_frame.get_height(),
            points.size(), count,
            min_x, max_x, min_y, max_y, min_z, max_z);
        }
      }

      pipeline.stop();
    } else {
      logger::info("No RealSense devices found");
    }
  });
  video_thread.detach();
}
