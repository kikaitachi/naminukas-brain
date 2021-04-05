#include <robotcontrol.h>

#include <chrono>
#include <vector>

#include "HardwareBeagleBoneBlue.hpp"
#include "Logger.hpp"

// bus for Robotics Cape and BeagleboneBlue is 2, interrupt pin is on gpio3.21
// change these for your platform
#define I2C_BUS 2
#define GPIO_INT_PIN_CHIP 3
#define GPIO_INT_PIN_PIN 21

static rc_mpu_data_t mpu_data;
static double yaw, pitch, roll;

static std::ofstream imu_log("/tmp/imu.csv", std::ios::out);

#define TAP_MAX_GAP_MS 800
static std::chrono::time_point<std::chrono::high_resolution_clock> last_tap_time =
  std::chrono::high_resolution_clock::now();
static int tap_count = 0;
static std::vector<int> taps;
static const char tap_codes[] = {
  'a', 'b', 'c', 'd', 'e',
  'f', 'g', 'h', 'i', 'j',
  'l', 'm', 'n', 'o', 'o',
  'q', 'r', 's', 't', 'u',
  'v', 'w', 'x', 'y', 'z'
};
static char tap_code = '\0';

BeagleBoneBluePneumatics::BeagleBoneBluePneumatics() {
  if (rc_motor_init() == -1) {
    logger::error("Failed to initialise H-bridge");
  }
}

BeagleBoneBluePneumatics::~BeagleBoneBluePneumatics() {
  if (rc_motor_cleanup() == -1) {
    logger::error("Failed to shutdown H-bridge");
  }
}

void BeagleBoneBluePneumatics::set_vacuum_pump_speed(double speed) {
  if (speed > 0) {
    if (rc_motor_set(1, speed) == -1) {
      logger::error("Failed to start vacuum pump");
    }
  } else {
    if (rc_motor_free_spin(1) == -1) {
      logger::error("Failed to stop vacuum pump");
    }
  }
}

void BeagleBoneBluePneumatics::left_foot_vent(bool vent) {
  // TODO: implement
}

void BeagleBoneBluePneumatics::right_foot_vent(bool vent) {
  // TODO: implement
}

static void on_imu_changed() {
  yaw = mpu_data.dmp_TaitBryan[TB_YAW_Z] * RAD_TO_DEG;
  pitch = mpu_data.dmp_TaitBryan[TB_PITCH_X] * RAD_TO_DEG;
  roll = mpu_data.dmp_TaitBryan[TB_ROLL_Y] * RAD_TO_DEG;
  imu_log
    << mpu_data.raw_accel[0] << "," << mpu_data.raw_accel[1] << "," << mpu_data.raw_accel[2] << ","
    // << mpu_data.tap_detected << "," << mpu_data.last_tap_direction << "," << mpu_data.last_tap_count
    << std::endl;
  if (tap_count > 0) {
    std::chrono::time_point<std::chrono::high_resolution_clock> now =
      std::chrono::high_resolution_clock::now();
    int elapsed_millis = std::chrono::duration_cast<std::chrono::milliseconds>
      (now - last_tap_time).count();
    if (elapsed_millis > TAP_MAX_GAP_MS) {
      taps.push_back(tap_count - 1);
      tap_count = 0;
      if (taps.size() == 2) {
        tap_code = tap_codes[taps[0] * 5 + taps[1]];
        taps.clear();
      }
    }
  }
}

static void tap_callback(int direction, int counter) {
  tap_count++;
  last_tap_time = std::chrono::high_resolution_clock::now();
  logger::info("Tap direction: %d, count: %d, code: %c", direction, counter, tap_code);
}

BeagleBoneBlueIMU::BeagleBoneBlueIMU() {
  rc_mpu_config_t mpu_config = rc_mpu_default_config();
  mpu_config.dmp_sample_rate = 200;
  mpu_config.dmp_fetch_accel_gyro = 1;  // provide raw data too
  mpu_config.tap_threshold = 5;
  mpu_config.orient = ORIENTATION_X_UP;
  mpu_config.i2c_bus = I2C_BUS;
  mpu_config.gpio_interrupt_pin_chip = GPIO_INT_PIN_CHIP;
  mpu_config.gpio_interrupt_pin = GPIO_INT_PIN_PIN;
  if (rc_mpu_initialize_dmp(&mpu_data, mpu_config)) {
    logger::error("Can't initialize inertial measurement unit");
  } else {
    rc_mpu_set_dmp_callback(&on_imu_changed);
    rc_mpu_set_tap_callback(&tap_callback);
  }
}

BeagleBoneBlueIMU::~BeagleBoneBlueIMU() {
  rc_mpu_power_off();
}

double BeagleBoneBlueIMU::get_yaw() {
  return yaw;
}

double BeagleBoneBlueIMU::get_pitch() {
  return pitch;
}

double BeagleBoneBlueIMU::get_roll() {
  return roll;
}

char BeagleBoneBlueIMU::get_tap_code() {
  char code = tap_code;
  tap_code == '\0';  // consume code on read
  return code;
}
