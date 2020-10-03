#include <iostream>
#include "dynamixel_sdk/dynamixel_sdk.h"

using namespace std;

int main(int argc, const char *argv[]) {
  dynamixel::PortHandler *portHandler = dynamixel::PortHandler::getPortHandler("/dev/ttyUSB0");
  if (portHandler->openPort()) {
    cout << "Succeeded to open the port" << endl;
  }
  else {
    cout << "Failed to open the port!" << endl;
  }

  return 0;
}
