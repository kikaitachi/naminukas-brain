#include "Hardware.hpp"

void hardware::Kinematics::add_position_listener(std::function<void(std::vector<JointPosition>&)> listener) {
    position_listeners.push_back(listener);
}
