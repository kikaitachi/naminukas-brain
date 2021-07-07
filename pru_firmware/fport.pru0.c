#include <stdint.h>
#include <pru_cfg.h>
#include <pru_iep.h>
#include "resource_table_empty.h"

volatile register unsigned int __R31;

#define CYCLES_PER_UART_BIT 1736

inline void reset_iep(void) {
  // Set counter to 0
  CT_IEP.TMR_CNT = 0x0;
  // Enable counter
  CT_IEP.TMR_GLB_CFG = 0x11;
}

inline int read_iep(void) {
  // Return counter content
  return CT_IEP.TMR_CNT;
}

inline int read_pin() {
  return __R31 & (1 << 15) ? 1 : 0;
}

uint8_t read_byte() {
  reset_iep();
  uint8_t result = 0;
  uint8_t sliding_bit = 1;
  int i;
  for (i = 1; i <= 8; i++) {
    while (read_iep() < CYCLES_PER_UART_BIT * i + CYCLES_PER_UART_BIT / 2) {}
    if (!read_pin()) {
      result |= sliding_bit;
    }
    sliding_bit <<= 1;
  }
  // Stop bit can be safely ignored
  return result;
}

void main(void) {
  // Clear SYSCFG[STANDBY_INIT] to enable OCP master port
  CT_CFG.SYSCFG_bit.STANDBY_INIT = 0;

  // Use then same address as librobotcontrol for quadrature encoder 4
  // in order not to mess up other PRU used for servo motor control
  uint32_t *channels = (uint32_t*)(0x10000 + 16 * 4);

  int i;
  uint8_t sbus_packet[24];
  uint8_t byte, len;
  for ( ; ; ) {
   restart:
    for () {
      reset_iep();
      // Wait for start bit
      while (read_pin() == 0) {
        if (read_iep() > CYCLES_PER_UART_BIT * 20) {
          // Too long until next bit, assume something is broken and restart
          goto restart;
        }
      }
    }
    byte = read_byte();
    if (byte != 0x7E) {
      // Invalid header
      goto restart;
    }
    len = read_byte();
    byte = read_byte();
    if (byte != 0) {
      // Not SBUS data
      reset_iep();
      while (read_iep() < CYCLES_PER_UART_BIT * (len + 2));
      goto restart;
    }
    for (i = 0; i < 25; i++) {
      byte = read_byte();
      if (byte == 0x7D) {
        byte = read_byte() ^ 0x20;
      }
      sbus_packet[i] = byte;
    }
    channels[ 0] = (sbus_packet[ 1]      | sbus_packet[ 2] << 8                        ) & 0x07FF;
    channels[ 1] = (sbus_packet[ 2]      | sbus_packet[ 3] << 5                        ) & 0x07FF;
    channels[ 2] = (sbus_packet[ 3] >> 6 | sbus_packet[ 4] << 2 | sbus_packet[ 5] << 10) & 0x07FF;
    channels[ 3] = (sbus_packet[ 5] >> 1 | sbus_packet[ 6] << 7                        ) & 0x07FF;
    channels[ 4] = (sbus_packet[ 6] >> 4 | sbus_packet[ 7] << 4                        ) & 0x07FF;
    channels[ 5] = (sbus_packet[ 7] >> 7 | sbus_packet[ 8] << 1 | sbus_packet[ 9] <<  9) & 0x07FF;
    channels[ 6] = (sbus_packet[ 9] >> 2 | sbus_packet[10] << 6                        ) & 0x07FF;
    channels[ 7] = (sbus_packet[10] >> 5 | sbus_packet[11] << 3                        ) & 0x07FF;
    channels[ 8] = (sbus_packet[12]      | sbus_packet[13] << 8                        ) & 0x07FF;
    channels[ 9] = (sbus_packet[13] >> 3 | sbus_packet[14] << 5                        ) & 0x07FF;
    channels[10] = (sbus_packet[14] >> 6 | sbus_packet[15] << 2 | sbus_packet[16] << 10) & 0x07FF;
    channels[11] = (sbus_packet[16] >> 1 | sbus_packet[17] << 7                        ) & 0x07FF;
    channels[12] = (sbus_packet[17] >> 4 | sbus_packet[18] << 4                        ) & 0x07FF;
    channels[13] = (sbus_packet[18] >> 7 | sbus_packet[19] << 1 | sbus_packet[20] <<  9) & 0x07FF;
    channels[14] = (sbus_packet[20] >> 2 | sbus_packet[21] << 6                        ) & 0x07FF;
    channels[15] = (sbus_packet[21] >> 5 | sbus_packet[22] << 3                        ) & 0x07FF;
  }
}
