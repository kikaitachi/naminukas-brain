#include "../Logger.hpp"
#include "RCRadio_PRU_SBUS.hpp"

#include <rc/pru.h>
#include <thread>

void RCRadio_PRU_SBUS::init(RCRadioChannelChangedHandler& change_handler) {
  std::thread poll_channels([] {
    volatile uint32_t* addr = rc_pru_shared_mem_ptr();
    if (addr == NULL) {
        logger::error("Can't map PRU memory. Program must be executed as root.");
        return;
    }
    for ( ; ; ) {
      // TODO: implement comparing channel values and calling callback
      logger::info("1: %d, 2: %d, 3: %d, 4: %d, 5: %d, 6: %d, 7: %d, 8: %d, 9: %d, 10: %d, 11: %d, 12: %d, 13: %d, 14: %d, 15: %d, 16: %d",
        addr[16], addr[17], addr[18], addr[19], addr[20], addr[21], addr[22], addr[23],
        addr[24], addr[25], addr[26], addr[27], addr[28], addr[28], addr[30], addr[31]);
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
  });
  poll_channels.detach();
}
