#include "../Logger.hpp"
#include "ActionSequential.hpp"

ActionSequential::ActionSequential(std::vector<std::shared_ptr<Action>> actions, int times)
    : actions(actions), times(times) {
  current_action_index = actions.size();
}

void ActionSequential::start() {
  logger::debug("Starting %d sequential actions", actions.size());
  current_action_index = 0;
  if (current_action_index < actions.size()) {
    logger::debug("Starting sequential action %d", current_action_index);
    actions[current_action_index]->start();
  }
}

void ActionSequential::abort() {
  if (current_action_index < actions.size()) {
    actions[current_action_index]->abort();
  }
  current_action_index = actions.size();
}

bool ActionSequential::execute() {
  logger::debug("Executing sequential action");
  if (current_action_index >= actions.size()) {
    if (times == LOOP_FOREVER) {
      current_action_index = 0;
      actions[current_action_index]->start();
    } else if (times > 0) {
      times--;
      if (times > 0) {
        current_action_index = 0;
        actions[current_action_index]->start();
      } else {
        return true;
      }
    } else {
      return true;
    }
  }
  if (actions[current_action_index]->execute()) {
    current_action_index++;
    if (current_action_index < actions.size()) {
      actions[current_action_index]->start();
    }
  }
  return false;
}
