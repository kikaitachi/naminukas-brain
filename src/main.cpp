#include <iostream>
#include "dynamixel_sdk/dynamixel_sdk.h"
#include "robotcontrol.h"

using namespace std;

int main(int argc, const char *argv[]) {
  if (rc_adc_init()) {
    cout << "Failed to run rc_init_adc()" << endl;
  } else {
    cout << "Battery voltage: " << rc_adc_batt() << endl;
  }

  dynamixel::PortHandler *portHandler = dynamixel::PortHandler::getPortHandler("/dev/ttyUSB0");
  if (portHandler->openPort()) {
    cout << "Succeeded to open the port" << endl;
  }
  else {
    cout << "Failed to open the port!" << endl;
  }

  return 0;
}
