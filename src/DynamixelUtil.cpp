#include <climits>
#include <cmath>

#include "DynamixelUtil.hpp"
#include "Logger.hpp"

DynamixelControlItem::DynamixelControlItem(int address, int size)
    : address(address), size(size) {
}

DynamixelControlValue::DynamixelControlValue(int id, int value)
    : id(id), value(value) {
}

int DynamixelXM430W350::positions_per_rotation() {
  return 4096;
}

int DynamixelXM430W350::rpm_to_value(double rpm) {
  return round(rpm / 0.229);
}

double DynamixelXM430W350::value_to_rpm(int value) {
  return value * 0.229;
}

double DynamixelXM430W350::value_to_current(int value) {
  return value * 0.00269;
}

int DynamixelXM430W350::pwm_to_value(double pwm) {
  return pwm * 885;
}

int DynamixelXM430W350::rpm2_to_value(double rpm2) {
  return round(rpm2 / 214.577);
}

DynamixelControlItem DynamixelXM430W350::drive_mode() {
  return DynamixelControlItem(10, 1);
}

DynamixelControlItem DynamixelXM430W350::operating_mode() {
  return DynamixelControlItem(11, 1);
}

DynamixelControlItem DynamixelXM430W350::max_position_limit() {
  return DynamixelControlItem(48, 4);
}

DynamixelControlItem DynamixelXM430W350::min_position_limit() {
  return DynamixelControlItem(52, 4);
}

DynamixelControlItem DynamixelXM430W350::torque_enable() {
  return DynamixelControlItem(64, 1);
}

DynamixelControlItem DynamixelXM430W350::goal_pwm() {
  return DynamixelControlItem(100, 2);
}

DynamixelControlItem DynamixelXM430W350::goal_current() {
  return DynamixelControlItem(102, 2);
}

DynamixelControlItem DynamixelXM430W350::goal_velocity() {
  return DynamixelControlItem(104, 4);
}

DynamixelControlItem DynamixelXM430W350::profile_acceleration() {
  return DynamixelControlItem(108, 4);
}

DynamixelControlItem DynamixelXM430W350::profile_velocity() {
  return DynamixelControlItem(112, 4);
}

DynamixelControlItem DynamixelXM430W350::goal_position() {
  return DynamixelControlItem(116, 4);
}

DynamixelControlItem DynamixelXM430W350::moving() {
  return DynamixelControlItem(122, 1);
}

DynamixelControlItem DynamixelXM430W350::moving_status() {
  return DynamixelControlItem(123, 1);
}

DynamixelControlItem DynamixelXM430W350::present_current() {
  return DynamixelControlItem(126, 2);
}

DynamixelControlItem DynamixelXM430W350::present_velocity() {
  return DynamixelControlItem(128, 4);
}

DynamixelControlItem DynamixelXM430W350::present_position() {
  return DynamixelControlItem(132, 4);
}

DynamixelControlItem DynamixelXM430W350::voltage() {
  return DynamixelControlItem(144, 2);
}

DynamixelControlItem DynamixelXM430W350::temperature() {
  return DynamixelControlItem(146, 1);
}

DynamixelControlItem DynamixelXM430W350::indirect_address(int offset) {
  return DynamixelControlItem(168 + offset, 2);
}

#define USB_LATENCY_FILE_NAME "/sys/bus/usb-serial/devices/ttyUSB0/latency_timer"
#define USB_LATENCY_VALUE 2

DynamixelConnection::DynamixelConnection(std::string device, float protocol, int baudrate) {
  // See http://emanual.robotis.com/docs/en/software/dynamixel/dynamixel_wizard2/#usb-latency-setting
  FILE *latency_timer = fopen(USB_LATENCY_FILE_NAME, "w");
  if (latency_timer == NULL) {
    logger::error("Failed to open latency time file %s", USB_LATENCY_FILE_NAME);
  } else {
    fprintf(latency_timer, "%d\n", USB_LATENCY_VALUE);
    fclose(latency_timer);
    logger::info("Updated USB latency to %d", USB_LATENCY_VALUE);
  }

  port_handler = dynamixel::PortHandler::getPortHandler(device.c_str());
  if (port_handler->openPort()) {
    logger::info("Succeeded to open Dynamixel port");
    packet_handler = dynamixel::PacketHandler::getPacketHandler(protocol);
    if (!port_handler->setBaudRate(baudrate)) {
      logger::error("Failed to change baudrate");
    }
  } else {
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

bool DynamixelConnection::write(DynamixelControlItem item, std::vector<DynamixelControlValue> values) {
  if (packet_handler == NULL) {
    return false;
  }
  const std::lock_guard<std::mutex> lock(mutex);
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
      return false;
    }
  }
  int dxl_comm_result = group_sync_write.txPacket();
  if (dxl_comm_result != COMM_SUCCESS) {
    logger::error("Sync write to address %d failed: %s",
      item.address, packet_handler->getTxRxResult(dxl_comm_result));
    return false;
  }
  return true;
}

std::vector<int> DynamixelConnection::read(DynamixelControlItem item, std::vector<int> ids) {
  std::vector<int> result;
  if (packet_handler == NULL) {
    return result;
  }
  const std::lock_guard<std::mutex> lock(mutex);
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
    } else {
      result.push_back(group_sync_read.getData(id, item.address, item.size));
    }
  }
  group_sync_read.clearParam();
  return result;
}

std::vector<std::vector<int>> DynamixelConnection::read(
    int address, std::vector<int> sizes, std::vector<int> ids) {
  std::vector<std::vector<int>> result;
  if (packet_handler == NULL) {
    return result;
  }
  int total_size = 0;
  for (int size : sizes) {
    total_size += size;
  }
  const std::lock_guard<std::mutex> lock(mutex);
  dynamixel::GroupSyncRead group_sync_read(port_handler, packet_handler, address, total_size);
  for (auto& id : ids) {
    if (!group_sync_read.addParam(id)) {
      logger::error("Failed to add parameter for reading from addresss %d of Dynamixel with id %d",
        address, id);
    }
  }
  int dxl_comm_result = group_sync_read.txRxPacket();
  if (dxl_comm_result != COMM_SUCCESS) {
    logger::error("Sync read from address %d failed: %s",
      address, packet_handler->getTxRxResult(dxl_comm_result));
  }
  for (auto& id : ids) {
    uint8_t dxl_error = 0;
    if (group_sync_read.getError(id, &dxl_error)) {
      logger::error("Failed to read address for Dynamixel with id %d: %s",
        address, id, packet_handler->getRxPacketError(dxl_error));
    }
    int current_address = address;
    int i = 0;
    std::vector<int> data;
    for (int size : sizes) {
      bool dxl_getdata_result = group_sync_read.isAvailable(id, current_address, size);
      if (dxl_getdata_result != true) {
        logger::error("Sync read doesn't have data at address %d for Dynamixel with id %d",
          current_address, id);
      } else {
        data.push_back(group_sync_read.getData(id, current_address, size));
      }
      current_address += size;
      i++;
    }
    result.push_back(data);
  }
  group_sync_read.clearParam();
  return result;
}
