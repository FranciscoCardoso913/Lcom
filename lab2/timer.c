#include <lcom/lcf.h>
#include <lcom/timer.h>

#include <stdint.h>

#include "i8254.h"

int (timer_set_frequency)(uint8_t timer, uint32_t freq) {
  /* To be implemented by the students */
  printf("%s is not yet implemented!\n", __func__);

  return 1;
}

int (timer_subscribe_int)(uint8_t *bit_no) {
    /* To be implemented by the students */
  printf("%s is not yet implemented!\n", __func__);

  return 1;
}

int (timer_unsubscribe_int)() {
  /* To be implemented by the students */
  printf("%s is not yet implemented!\n", __func__);

  return 1;
}

void (timer_int_handler)() {
  /* To be implemented by the students */
  printf("%s is not yet implemented!\n", __func__);
}

int (timer_get_conf)(uint8_t timer, uint8_t *st) {
  uint8_t ctrlWord = TIMER_RB_CMD | TIMER_RB_COUNT_;
  uint8_t port;
  int err;

  switch(timer) {
    case 0:
      ctrlWord |= BIT(1);
      port = TIMER_0;
      break;
    case 1:
      ctrlWord |= BIT(2);
      port = TIMER_1;
      break;
    case 2:
      ctrlWord |= BIT(3);
      port = TIMER_2;
      break;
    default:
      fprintf(stderr, "Wrong timer input (0-2): %d\n", timer);
      return 1;
  }

  err = sys_outb(TIMER_CTRL, ctrlWord);

  if (err) {
    fprintf(stderr, "Error when writing control word\n");
    return err;
  }

  err = util_sys_inb(port, st);

  return err;
}
int (timer_display_conf)(uint8_t timer, uint8_t st,
                        enum timer_status_field field) {
  /* To be implemented by the students */
  printf("%s is not yet implemented!\n", __func__);

  return 1;
}
