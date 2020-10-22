#ifndef NAMINUKAS_BRAIN_ROBOT_H_
#define NAMINUKAS_BRAIN_ROBOT_H_

#include <mutex>

class Robot {
  public:
    static void lockState();
    static void unlockState();

  private:
    static std::mutex stateMutex;
};

#endif  // NAMINUKAS_BRAIN_ROBOT_H_
