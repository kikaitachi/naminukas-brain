#include <cmath>
#include <fstream>
#include <limits>
#include <string>
#include <thread>
#include <time.h>
#include "webp/encode.h"

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

static const std::string TIMELAPSE_DIR = "/tmp/";

/**
 * ffmpeg -f concat -safe 0 -i timelapse-rgb.txt timelapse-rgb.mp4
 * ffmpeg -f concat -safe 0 -i timelapse-depth.txt timelapse-depth.mp4
 * ffmpeg -f concat -safe 0 -i timelapse-infrared.txt timelapse-infrared.mp4
 */
static void image2file(std::string file_name, const uint8_t* rgb, int width, int height, int stride) {
  uint8_t* output;
  size_t size = WebPEncodeRGB(rgb, width, height, stride, 75, &output);
  //std::ofstream file(file_name, std::ios::out | std::ios::binary);
  //file.write((const char *)output, size);
  WebPFree(output);
}

PointCloud::PointCloud(telemetry::Items& telemetryItems, std::function<bool()> is_terminated) {
  points_telemetry = new telemetry::ItemPoints(telemetry::ROOT_ITEM_ID, "Point cloud", {});
  telemetry::Item* machine = telemetryItems.add_item(points_telemetry);

  std::thread video_thread([=]() {
    int frame_index = 0;
    std::ofstream timelapse_rgb(TIMELAPSE_DIR + "timelapse-rgb.txt", std::ios::out);
    std::ofstream timelapse_depth(TIMELAPSE_DIR + "timelapse-depth.txt", std::ios::out);
    std::ofstream timelapse_infrared(TIMELAPSE_DIR + "timelapse-infrared.txt", std::ios::out);

    rs2::context ctx;

    rs2::device_list device_list = ctx.query_devices();

    if (device_list.size() > 0) {
      rs2::device dev = device_list[0];

      logger::info("Using RealSense device 0: %s", dev.get_info(RS2_CAMERA_INFO_NAME));

      rs2::pipeline pipeline(ctx);

      rs2::config config;

      //config.enable_stream(RGB_STREAM, RGB_STREAM_INDEX, RGB_WIDTH, RGB_HEIGHT, RGB_FORMAT, RGB_FPS);
      config.enable_stream(DEPTH_STREAM, DEPTH_STREAM_INDEX, DEPTH_WIDTH, DEPTH_HEIGHT, DEPTH_FORMAT, DEPTH_FPS);
      //config.enable_stream(INFRARED_STREAM, INFRARED_STREAM_INDEX, INFRARED_WIDTH, INFRARED_HEIGHT, INFRARED_FORMAT, INFRARED_FPS);

      rs2::pointcloud pc;
      rs2::colorizer color_map;

      pipeline.start(config);

      //struct timespec last_frame_time;

      while (!is_terminated()) {
        rs2::frameset frameset;
        if (pipeline.try_wait_for_frames(&frameset, 1000000)) {
          /*struct timespec now;
          clock_gettime(CLOCK_REALTIME, &now);
          if (frame_index != 0) {
            double duration = now.tv_sec - last_frame_time.tv_sec + (now.tv_nsec - last_frame_time.tv_nsec) / 1000000000.0 - 1 / 25.0;
            timelapse_rgb << "duration " << duration << std::endl;
            timelapse_depth << "duration " << duration << std::endl;
            timelapse_infrared << "duration " << duration << std::endl;
          }
          std::string file_name_rgb = "rgb" + std::to_string(frame_index) + ".webp";
          std::string file_name_depth = "depth" + std::to_string(frame_index) + ".webp";
          std::string file_name_infrared = "infrared" + std::to_string(frame_index) + ".webp";
          timelapse_rgb << "file '" << file_name_rgb << "'" << std::endl;
          timelapse_depth << "file '" << file_name_depth << "'" << std::endl;
          timelapse_infrared << "file '" << file_name_infrared << "'" << std::endl;
          frame_index++;
          last_frame_time = now;*/

          //auto color_frame = frameset.get_color_frame();
          rs2::depth_frame depth_frame = frameset.get_depth_frame();

          for (auto listener : depth_listeners) {
            listener(depth_frame);
          }
          //auto infrared_frame = frameset.get_infrared_frame();

          //int bytes_per_pixel = color_frame.get_bytes_per_pixel(); // Get # of bytes per pixel
          //int stride_in_bytes = color_frame.get_stride_in_bytes(); // Get line width in bytes
          //const auto texture = reinterpret_cast<const uint8_t*>(color_frame.get_data());

          //const auto infrared = reinterpret_cast<const uint8_t*>(infrared_frame.get_data());
          //const auto colored_depth_frame = color_map.process(depth_frame);
          //const auto colored_depth = reinterpret_cast<const uint8_t*>(colored_depth_frame.get_data());

          //image2file(TIMELAPSE_DIR + file_name_rgb, texture, RGB_WIDTH, RGB_HEIGHT, stride_in_bytes);
          //image2file(TIMELAPSE_DIR + file_name_depth, colored_depth, DEPTH_WIDTH, DEPTH_HEIGHT, DEPTH_WIDTH * 3);
          //image2file(TIMELAPSE_DIR + file_name_infrared, infrared, INFRARED_WIDTH, INFRARED_HEIGHT, infrared_frame.get_stride_in_bytes());

          /*pc.map_to(color_frame);
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
            min_x, max_x, min_y, max_y, min_z, max_z);*/
        }
      }

      pipeline.stop();
    } else {
      logger::info("No RealSense devices found");
    }
  });
  video_thread.detach();
}

void PointCloud::add_depth_listener(std::function<void(rs2::depth_frame&)> listener) {
  depth_listeners.push_back(listener);
}

void PointCloud::remove_depth_listener(std::function<void(rs2::depth_frame&)> listener) {
  // TODO: implement
}
