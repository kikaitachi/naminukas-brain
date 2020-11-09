#include "BeagleBoneBlueIMU.hpp"
#include "Logger.hpp"

// bus for Robotics Cape and BeagleboneBlue is 2, interrupt pin is on gpio3.21
// change these for your platform
#define I2C_BUS 2
#define GPIO_INT_PIN_CHIP 3
#define GPIO_INT_PIN_PIN 21

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
		rc_mpu_set_dmp_callback([]() {
      //state->yaw = mpu_data.dmp_TaitBryan[TB_YAW_Z] * RAD_TO_DEG;
	    //state->pitch = mpu_data.dmp_TaitBryan[TB_PITCH_X] * RAD_TO_DEG;
	    //state->roll = mpu_data.dmp_TaitBryan[TB_ROLL_Y] * RAD_TO_DEG;
    });
	}
}

BeagleBoneBlueIMU::~BeagleBoneBlueIMU() {
	rc_mpu_power_off();
}

double BeagleBoneBlueIMU::yaw() {
  //
}

double BeagleBoneBlueIMU::pitch() {
  //
}

double BeagleBoneBlueIMU::roll() {
  //
}
