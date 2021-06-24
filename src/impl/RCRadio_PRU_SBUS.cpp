#include "../Logger.hpp"
#include "RCRadio_PRU_SBUS.hpp"

#include <rc/pru.h>
#include <thread>

#define CHANNELS 8
#define OFFSET 16

void RCRadio_PRU_SBUS::init(RCRadioChannelChangedHandler& change_handler) {
  std::thread poll_channels([&] {
    volatile uint32_t* addr = rc_pru_shared_mem_ptr();
    if (addr == NULL) {
        logger::error("Can't map PRU memory. Program must be executed as root.");
        return;
    }
    uint32_t channels[CHANNELS];
    for (int i = 0; i < CHANNELS; i++) {
      channels[i] = addr[OFFSET + i] = 0;
    }
    for ( ; ; ) {
      for (int i = 0; i < CHANNELS; i++) {
        uint32_t new_value = addr[OFFSET + i];
        if (channels[i] != new_value) {
          channels[i] = new_value;
          change_handler.on_rc_radio_channel_change(i, new_value);
        }
      }
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
  });
  poll_channels.detach();
}