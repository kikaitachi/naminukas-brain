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
  if (rc_spi_init_manual_slave(RC_BB_SPI1_SS1, SPI_MODE_0, RC_SPI_MAX_SPEED, RC_BLUE_SS1_GPIO) == -1) {
    logger::error("Failed to initialize SPI for left pressure sensor");
  }
  if (rc_spi_init_manual_slave(RC_BB_SPI1_SS2, SPI_MODE_0, RC_SPI_MAX_SPEED, RC_BLUE_SS1_GPIO) == -1) {
    logger::error("Failed to initialize SPI for right pressure sensor");
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

double BeagleBoneBluePneumatics::get_pressure_left() {
  uint8_t tx_buffer[4] = { 0xAA, 0, 0, 0 };
  uint8_t rx_buffer[4];
  if (rc_spi_transfer(RC_BB_SPI1_SS1, tx_buffer, 3, rx_buffer) == -1) {
    logger::error("Failed to read from left foot pressure sensor via SPI: init command");
    return -1;
  }
  rc_usleep(5);
  tx_buffer[0] = 0xF0;
  if (rc_spi_transfer(RC_BB_SPI1_SS1, tx_buffer, 4, rx_buffer) == -1) {
    logger::error("Failed to read from left foot pressure sensor via SPI: data command");
    return -1;
  }
  double p_min = 0;
  double p_max = 15;
  int output_min = 1677722;
  int output_max = 0xE66666;
  int output = rx_buffer[1] << 16 | rx_buffer[2] << 8 | rx_buffer[3];
  return (output - output_min) * (p_max - p_min) / (output_max - output_min) + p_min;
}

double BeagleBoneBluePneumatics::get_pressure_right() {
  //
}

static void on_imu_changed() {
  yaw = mpu_data.dmp_TaitBryan[TB_YAW_Z] * RAD_TO_DEG;
  pitch = mpu_data.dmp_TaitBryan[TB_PITCH_X] * RAD_TO_DEG;
  roll = mpu_data.dmp_TaitBryan[TB_ROLL_Y] * RAD_TO_DEG;
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
        logger::info("Tap code: %c", tap_code);
      }
    }
  }
}

static void tap_callback(int direction, int counter) {
  tap_count++;
  last_tap_time = std::chrono::high_resolution_clock::now();
  logger::info("Tap direction: %d, count: %d", direction, counter);
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

BeagleBoneBlueBarometer::BeagleBoneBlueBarometer() {
  if (rc_bmp_init(BMP_OVERSAMPLE_1, BMP_FILTER_OFF)) {
    logger::error("Can't initialize barometer");
  }
}

double BeagleBoneBlueBarometer::get_pressure() {
  rc_bmp_data_t barometer_data;
  if (rc_bmp_read(&barometer_data) == -1) {
    logger::error("Failed to read barometer data");
    return 0;
  }
  return barometer_data.pressure_pa;
}
