#include <thread>
#include <librealsense2/rs.hpp>

#include "Logger.hpp"
#include "PointCloud.hpp"

static void check_error(rs2_error* e) {
	if (e) {
		logger::error("rs_error was raised when calling %s(%s): %s",
			rs2_get_failed_function(e), rs2_get_failed_args(e), rs2_get_error_message(e));
	}
}

#define DEPTH_STREAM       RS2_STREAM_DEPTH
#define DEPTH_FORMAT       RS2_FORMAT_Z16
#define DEPTH_WIDTH        480
#define DEPTH_HEIGHT       270
#define DEPTH_FPS          6
#define DEPTH_STREAM_INDEX -1

#define RGB_STREAM       RS2_STREAM_COLOR
#define RGB_FORMAT       RS2_FORMAT_BGR8
#define RGB_WIDTH        424
#define RGB_HEIGHT       240
#define RGB_FPS          6
#define RGB_STREAM_INDEX -1

PointCloud::PointCloud(std::function<bool()> is_terminated) {
  std::thread video_thread([=]() {
    rs2_error* e = NULL;

    rs2_context* ctx = rs2_create_context(RS2_API_VERSION, &e);
    check_error(e);

    rs2_device_list* device_list = rs2_query_devices(ctx, &e);
    check_error(e);

    int dev_count = rs2_get_device_count(device_list, &e);
    check_error(e);
    if (dev_count > 0) {
      rs2_device* dev = rs2_create_device(device_list, 0, &e);
      check_error(e);

      const char* camera_name = rs2_get_device_info(dev, RS2_CAMERA_INFO_NAME, &e);
      check_error(e);
      logger::info("Using RealSense device 0: %s", camera_name);

      rs2_pipeline* pipeline = rs2_create_pipeline(ctx, &e);
      check_error(e);

      rs2_config* config = rs2_create_config(&e);
      check_error(e);

      rs2_config_enable_stream(config, RGB_STREAM, RGB_STREAM_INDEX, RGB_WIDTH, RGB_HEIGHT, RGB_FORMAT, RGB_FPS, &e);
      check_error(e);
      rs2_config_enable_stream(config, DEPTH_STREAM, DEPTH_STREAM_INDEX, DEPTH_WIDTH, DEPTH_HEIGHT, DEPTH_FORMAT, DEPTH_FPS, &e);
      check_error(e);

      rs2_pipeline_profile* pipeline_profile = rs2_pipeline_start_with_config(pipeline, config, &e);
      check_error(e);

      rs2::pointcloud pc;

      while (!is_terminated()) {
        rs2_frame* frames = NULL;
        if (rs2_pipeline_try_wait_for_frames(pipeline, &frames, 1000000, &e)) {
          check_error(e);

          int num_of_frames = rs2_embedded_frames_count(frames, &e);
		      check_error(e);
          logger::debug("Got %d frames", num_of_frames);

          for (int i = 0; i < num_of_frames; i++)	{
            rs2_frame* frame = rs2_extract_frame(frames, i, &e);
            check_error(e);

            // Get the depth frame's dimensions
            int width = rs2_get_frame_width(frame, &e);
            check_error(e);
            int height = rs2_get_frame_height(frame, &e);
            check_error(e);

            // Check if the given frame can be extended to depth frame interface
            // Accept only depth frames and skip other frames
            if (0 == rs2_is_frame_extendable_to(frame, RS2_EXTENSION_DEPTH_FRAME, &e)) {
              //pc.map_to(frame);
              logger::warn("Not depth frame %dx%d received", width, height);
            } else {
              rs2::points points = pc.calculate(frame);
              logger::debug("Got %d points for frame %dx%d", points.size(), width, height);
            }
            rs2_release_frame(frame);
          }

          rs2_release_frame(frames);
        }
      }

      rs2_pipeline_stop(pipeline, &e);
      check_error(e);

      rs2_delete_pipeline_profile(pipeline_profile);
      rs2_delete_config(config);
      rs2_delete_pipeline(pipeline);
      rs2_delete_device(dev);
    } else {
      logger::info("No RealSense devices found");
    }
    rs2_delete_device_list(device_list);
    rs2_delete_context(ctx);
  });
  video_thread.detach();
}
