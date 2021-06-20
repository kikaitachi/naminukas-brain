#include <stdint.h>
#include <pru_cfg.h>
#include "resource_table_empty.h"
#include "prugpio.h"

volatile register unsigned int __R31;

void main(void) {
  // Use then same address as librobot control for quadrature encoder 4
  uint32_t *addr = (uint32_t*)(0x10000 + 16 * 4);
  *addr = 0;
  uint32_t mask = 0x8000;
  for ( ; ;) {
    uint32_t bits = __R31;
    *addr = bits & mask;
  }
}
