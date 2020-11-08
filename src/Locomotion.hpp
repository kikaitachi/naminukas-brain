#ifndef NAMINUKAS_BRAIN_LOCOMOTION_H_
#define NAMINUKAS_BRAIN_LOCOMOTION_H_

#include <string>

#include "Hardware.hpp"

class Locomotion {
  public:
    virtual std::string name() = 0;
    virtual void start();
    virtual void stop();
    virtual void up();
    virtual void down();
    virtual void left();
    virtual void right();
};

#endif // NAMINUKAS_BRAIN_LOCOMOTION_H_
