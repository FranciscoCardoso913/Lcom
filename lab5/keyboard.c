#include <lcom/lcf.h>

#include <stdint.h>

#include "i8042.h"
#include "keyboard.h"


extern int keyboard_hook_id;
extern uint8_t scan_code;
extern uint8_t status;
extern int err;
void 	(kbc_ih) (){
    
    if(util_sys_inb(0x60,&scan_code)) err=1;
    if(status & (BIT(7)|BIT(6))) err=1;

}
int (keyboard_subscribe_int)(uint8_t *bit_no) {
  *bit_no = keyboard_hook_id;
  if (sys_irqsetpolicy(KBD_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &keyboard_hook_id)) {
    fprintf(stderr, "Error while setting kbd subscription\n");
    return 1;
  }


  return 0;
}

int (keyboard_unsubscribe_int)() {
  if(sys_irqrmpolicy(&keyboard_hook_id)){
    printf("Error in sys_irqsetpolicy");
    return 1;
  };

  return 0;
}

int (kbd_print_scancode) (bool make, uint8_t size, uint8_t *bytes);
int (kbd_print_no_sysinb) (uint32_t cnt);


