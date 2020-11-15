#ifndef NAMINUKAS_BRAIN_LOCOMOTION_H_
#define NAMINUKAS_BRAIN_LOCOMOTION_H_

#include <string>

#include "Hardware.hpp"

class Locomotion {
  public:
    virtual std::string name() = 0;
    virtual void start();
    virtual void stop();
    virtual void up(bool key_down);
    virtual void down(bool key_down);
    virtual void left(bool key_down);
    virtual void right(bool key_down);
};

#endif // NAMINUKAS_BRAIN_LOCOMOTION_H_
