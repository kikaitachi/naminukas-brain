#include "DynamixelUtil.hpp"

DynamixelControlItem::DynamixelControlItem(int address, int size)
    : address(address), size(size) {
}

DynamixelControlItem DynamixelXM430W350::torque_enable() {
  return DynamixelControlItem(64, 1);
}
