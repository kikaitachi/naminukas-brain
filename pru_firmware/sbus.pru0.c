#include <stdint.h>
#include <pru_cfg.h>
#include <pru_iep.h>
#include "resource_table_empty.h"

volatile register unsigned int __R31;

#define CYCLES_PER_UART_BIT 2000

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
  int i;
  for (i = 1; i <= 8; i++) {
    while (read_iep() < CYCLES_PER_UART_BIT * i + CYCLES_PER_UART_BIT / 2) {}
    result = (result << 1) | read_pin();
  }
  // Ignore parity for now
  while (read_iep() < CYCLES_PER_UART_BIT * 9 + CYCLES_PER_UART_BIT / 2) { }
  // 2 stop bits can be safely ignored
  return result;
}

void main(void) {
  // Clear SYSCFG[STANDBY_INIT] to enable OCP master port
  CT_CFG.SYSCFG_bit.STANDBY_INIT = 0;

  // Use then same address as librobot control for quadrature encoder 4
  // in order not to mess up other PRU used for servo motor control
  uint32_t *addr = (uint32_t*)(0x10000 + 16 * 4);
  *addr = 0;

  int i;
  uint8_t sbus_packet[25];
  for ( ; ; ) {
   restart:
    for (i = 0; i < 25; i++) {
      reset_iep();
      // Wait for start bit
      while (!read_pin()) {
        if (read_iep() > CYCLES_PER_UART_BIT * 20) {
          goto restart;
        }
      }
      sbus_packet[i] = read_byte();
    }
    //if (sbus_packet[24] == 0) {
      uint32_t byte1 = sbus_packet[1];
      uint32_t byte2 = sbus_packet[2];
      *addr = (byte1 << 3) | (byte2 >> 5);
    //}
    *addr = sbus_packet[24]; // Header 0x0F
  }
}
