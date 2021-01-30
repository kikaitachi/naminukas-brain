#ifndef NAMINUKAS_BRAIN_ACTION_SEQUENTIAL_H_
#define NAMINUKAS_BRAIN_ACTION_SEQUENTIAL_H_

#include <vector>

#include "Action.hpp"

class ActionSequential : public Action {
  public:
    ActionSequential(std::vector<Action> actions);
    void start();
    void abort();
    bool execute();

  private:
    std::vector<Action> actions;
    int current_action_index;
};

#endif  // NAMINUKAS_BRAIN_ACTION_SEQUENTIAL_H_
