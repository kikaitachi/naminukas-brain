#include <stdint.h>
#include <pru_cfg.h>
#include "resource_table_empty.h"
#include "prugpio.h"

volatile register unsigned int __R31;

inline int read_pin() {
  uint32_t bits = __R31;
  uint32_t mask = 0x8000;
  return bits & mask ? 0 : 1;
}

inline uint8_t read_byte() {
  // Wait for start bit
  while (!read_pin()) {
  }
  uint8_t result = 0;
  int i;
  for (i = 0; i < 8; i++) {
    __delay_cycles(10000 / 5);  // UART clock 10000ns / 5ns per cycle
    result = (result << 1) | read_pin();
  }
  __delay_cycles(10000 / 5);  // Ignore parity for now
  // 2 stop bits can be safely ignored
  return result;
}

void main(void) {
  // Use then same address as librobot control for quadrature encoder 4
  uint32_t *addr = (uint32_t*)(0x10000 + 16 * 4);
  *addr = 0;
  for ( ; ; ) {
    read_byte();  // header
    uint32_t byte1 = read_byte();
    uint32_t byte2 = read_byte();
    *addr = byte1 << 3 | byte2 >> 5;
    int i;
    for (i = 0; i < 22; i++) {
      read_byte();
    }
  }
}
