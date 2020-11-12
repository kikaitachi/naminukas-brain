#include <climits>

#include "DynamixelUtil.hpp"
#include "Logger.hpp"

DynamixelControlItem::DynamixelControlItem(int address, int size)
    : address(address), size(size) {
}

DynamixelControlValue::DynamixelControlValue(int id, int value)
    : id(id), value(value) {
}

DynamixelControlItem DynamixelXM430W350::drive_mode() {
  return DynamixelControlItem(10, 1);
}

DynamixelControlItem DynamixelXM430W350::operating_mode() {
  return DynamixelControlItem(11, 1);
}

DynamixelControlItem DynamixelXM430W350::torque_enable() {
  return DynamixelControlItem(64, 1);
}

DynamixelConnection::DynamixelConnection(std::string device, float protocol, int baudrate) {
  port_handler = dynamixel::PortHandler::getPortHandler(device.c_str());
  if (port_handler->openPort()) {
    logger::info("Succeeded to open Dynamixel port");
    packet_handler = dynamixel::PacketHandler::getPacketHandler(protocol);
    if (!port_handler->setBaudRate(baudrate)) {
      logger::error("Failed to change baudrate");
    }
  }
  else {
    packet_handler = NULL;
    logger::error("Failed to open Dynamixel port");
  }
}

DynamixelConnection::~DynamixelConnection() {
  port_handler->closePort();
  if (packet_handler != NULL) {
    delete packet_handler;
  }
  delete port_handler;
}

bool DynamixelConnection::write(DynamixelControlItem item, std::initializer_list<DynamixelControlValue> values) {
  dynamixel::GroupSyncWrite group_sync_write(port_handler, packet_handler, item.address, item.size);
  for (auto& value : values) {
    uint8_t data[4];
    data[0] = DXL_LOBYTE(DXL_LOWORD(value.value));
    data[1] = DXL_HIBYTE(DXL_LOWORD(value.value));
    data[2] = DXL_LOBYTE(DXL_HIWORD(value.value));
    data[3] = DXL_HIBYTE(DXL_HIWORD(value.value));
    if (!group_sync_write.addParam(value.id, data)) {
      logger::error("Failed to add value %d for writing to addresss %d of Dynamixel with id %d",
        value.value, item.address, value.id);
    }
  }
  int dxl_comm_result = group_sync_write.txPacket();
  if (dxl_comm_result != COMM_SUCCESS) {
    logger::error("Sync write to address %d failed: %s",
      item.address, packet_handler->getTxRxResult(dxl_comm_result));
  }
  group_sync_write.clearParam();
}

std::vector<int> DynamixelConnection::read(DynamixelControlItem item, std::initializer_list<int> ids) {
  std::vector<int> result(ids.size());
  dynamixel::GroupSyncRead group_sync_read(port_handler, packet_handler, item.address, item.size);
  for (auto& id : ids) {
    if (!group_sync_read.addParam(id)) {
      logger::error("Failed to add parameter for reading from addresss %d of Dynamixel with id %d",
        item.address, id);
    }
  }
  int dxl_comm_result = group_sync_read.txRxPacket();
  if (dxl_comm_result != COMM_SUCCESS) {
    logger::error("Sync read from address %d failed: %s",
      item.address, packet_handler->getTxRxResult(dxl_comm_result));
  }
  for (auto& id : ids) {
    uint8_t dxl_error = 0;
    if (group_sync_read.getError(id, &dxl_error)) {
      logger::error("Failed to read address for Dynamixel with id %d: %s",
        item.address, id, packet_handler->getRxPacketError(dxl_error));
    }
    bool dxl_getdata_result = group_sync_read.isAvailable(id, item.address, item.size);
    if (dxl_getdata_result != true) {
      logger::error("Sync read from address %d failed for Dynamixel with id %d",
        item.address, id);
      result.push_back(INT_MIN);
    } else {
      result.push_back(group_sync_read.getData(id, item.address, item.size));
    }
  }
  group_sync_read.clearParam();
  return result;
}
