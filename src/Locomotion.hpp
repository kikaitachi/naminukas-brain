#ifndef NAMINUKAS_BRAIN_LOCOMOTION_H_
#define NAMINUKAS_BRAIN_LOCOMOTION_H_

#include <string>
#include <thread>

#include "Hardware.hpp"

class Locomotion {
  public:
    Locomotion(int control_loop_frequency);
    virtual ~Locomotion();
    virtual std::string name() = 0;
    virtual void control_loop();
    void start();
    void stop();
    virtual void on_start();
    virtual void on_stop();
    virtual void up(bool key_down);
    virtual void down(bool key_down);
    virtual void left(bool key_down);
    virtual void right(bool key_down);

  protected:
    bool stopped = true;

  private:
    int control_loop_frequency;
    std::thread* control_loop_thread;
};

#endif // NAMINUKAS_BRAIN_LOCOMOTION_H_
