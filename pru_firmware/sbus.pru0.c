#include <stdint.h>
#include <pru_cfg.h>
#include "resource_table_empty.h"
#include "prugpio.h"
#include <pru_iep.h>

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
  uint32_t bits = __R31;
  uint32_t mask = 0x8000;
  return bits & mask ? 0 : 1;
}

inline uint8_t read_byte() {
  reset_iep();
  uint8_t result = 0;
  int i;
  for (i = 0; i < 8; i++) {
    while (read_iep() < CYCLES_PER_UART_BIT * (i + 1)) { }
    result = (result << 1) | read_pin();
  }
  // Ignore parity for now
  while (read_iep() < CYCLES_PER_UART_BIT * 8) { }
  // 2 stop bits can be safely ignored
  return result;
}

void main(void) {
  // Clear SYSCFG[STANDBY_INIT] to enable OCP master port
  CT_CFG.SYSCFG_bit.STANDBY_INIT = 0;

  // Use then same address as librobot control for quadrature encoder 4
  uint32_t *addr = (uint32_t*)(0x10000 + 16 * 4);
  *addr = 0;

  int i;
  uint8_t sbus_packet[25];
  for ( ; ; ) {
   restart:
    for (i = 0; i < 25; i++) {
      reset_iep();
      // Wait for start bit
      while (!read_pin()) { }
      if (i != 0 && read_iep() > CYCLES_PER_UART_BIT * 5) {
        // If is not the first byte and start bit took longer than 5 clock cycles 
        goto restart;
      }
      sbus_packet[i] = read_byte();
    }
    uint8_t header = read_byte();  // header
    uint32_t byte1 = read_byte();
    uint32_t byte2 = read_byte();
    //*addr = byte1 << 3 | byte2 >> 5;
    *addr = sbus_packet[0]; // Header 0x0F
  }
}
