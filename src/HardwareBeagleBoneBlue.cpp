#include <robotcontrol.h>

#include "HardwareBeagleBoneBlue.hpp"
#include "Logger.hpp"

// bus for Robotics Cape and BeagleboneBlue is 2, interrupt pin is on gpio3.21
// change these for your platform
#define I2C_BUS 2
#define GPIO_INT_PIN_CHIP 3
#define GPIO_INT_PIN_PIN 21

static rc_mpu_data_t mpu_data;
static double yaw, pitch, roll;

static void on_imu_changed() {
  yaw = mpu_data.dmp_TaitBryan[TB_YAW_Z] * RAD_TO_DEG;
  pitch = mpu_data.dmp_TaitBryan[TB_PITCH_X] * RAD_TO_DEG;
  roll = mpu_data.dmp_TaitBryan[TB_ROLL_Y] * RAD_TO_DEG;
}

BeagleBoneBlueIMU::BeagleBoneBlueIMU() {
  rc_mpu_config_t mpu_config = rc_mpu_default_config();
  mpu_config.dmp_sample_rate = 200;
  mpu_config.orient = ORIENTATION_X_UP;
  mpu_config.i2c_bus = I2C_BUS;
  mpu_config.gpio_interrupt_pin_chip = GPIO_INT_PIN_CHIP;
  mpu_config.gpio_interrupt_pin = GPIO_INT_PIN_PIN;
  if (rc_mpu_initialize_dmp(&mpu_data, mpu_config)) {
    logger::error("Can't initialize inertial measurement unit");
  } else {
    rc_mpu_set_dmp_callback(&on_imu_changed);
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
