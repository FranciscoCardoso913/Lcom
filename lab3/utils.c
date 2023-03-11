#include <lcom/lcf.h>

#include <stdint.h>

#ifdef LAB3

int cnt = 0;

#endif

int(util_get_LSB)(uint16_t val, uint8_t *lsb) {
  *lsb = (uint8_t) val;

  return 0; 
}

int(util_get_MSB)(uint16_t val, uint8_t *msb) {
  *msb = (uint8_t) (val >> 8);

  return 0;
}

int (util_sys_inb)(int port, uint8_t *value) {
  uint32_t v;
  int err;

  err = sys_inb(port, &v);

  if (err) {
    fprintf(stderr, "Error %d when executing sysin_b\n", err);
    return err;
  }

  *value = (uint8_t) v;

  #ifdef LAB3

  cnt++;

  #endif

  return 0;
}
