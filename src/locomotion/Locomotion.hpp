#ifndef NAMINUKAS_BRAIN_LOCOMOTION_H_
#define NAMINUKAS_BRAIN_LOCOMOTION_H_

#include <set>
#include <string>
#include <thread>

#include "../geom/Pose.hpp"
#include "../Hardware.hpp"
#include "../Logger.hpp"

class Locomotion {
  public:
    Locomotion(int control_loop_frequency);
    virtual ~Locomotion();
    virtual std::string name() = 0;
    virtual Pose control_loop(Pose pose);
    virtual void start();
    virtual void stop();
    virtual void on_start();
    virtual void on_stop();
    virtual void up(bool key_down, std::set<std::string>& modifiers);
    virtual void down(bool key_down, std::set<std::string>& modifiers);
    virtual void left(bool key_down, std::set<std::string>& modifiers);
    virtual void right(bool key_down, std::set<std::string>& modifiers);

  protected:
    bool stopped = true;
    const double initial_ankle_angle = 360.0 / 16;
    uint64_t control_loop_nanos;

  private:
    int control_loop_frequency;
    std::thread* control_loop_thread;
};

#endif // NAMINUKAS_BRAIN_LOCOMOTION_H_
