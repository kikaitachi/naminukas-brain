#ifndef NAMINUKAS_BRAIN_ACTION_SEQUENTIAL_H_
#define NAMINUKAS_BRAIN_ACTION_SEQUENTIAL_H_

#include <memory>
#include <vector>

#include "Action.hpp"

class ActionSequential : public Action {
  public:
    ActionSequential(std::vector<std::shared_ptr<Action>> actions, int times = 1);
    void start();
    void abort();
    bool execute();

    static const int LOOP_FOREVER = -1;

  private:
    std::vector<std::shared_ptr<Action>> actions;
    int current_action_index;
    int times;
};

#endif  // NAMINUKAS_BRAIN_ACTION_SEQUENTIAL_H_
