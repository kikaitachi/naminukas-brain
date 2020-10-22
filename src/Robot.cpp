#include "Robot.hpp"

std::mutex Robot::stateMutex;

void Robot::lockState() {
  stateMutex.lock();
}

void Robot::unlockState() {
  stateMutex.unlock();
}
