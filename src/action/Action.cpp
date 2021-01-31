#include "../Logger.hpp"
#include "Action.hpp"

void Action::start() {
  logger::debug("Start empty action");
}

void Action::abort() {
}

bool Action::execute() {
  return true;
}
