#include <lcom/lcf.h>

#include "mouse.h"
#include "i8042.h"
#include "../lab2/i8254.h"


int timer_hook_id = 0;
int mouse_hook_id = 1;
extern uint8_t status;
int timer_counter = 0;

int mouse_subscribe_int(uint8_t *bit_no) {
  *bit_no = BIT(mouse_hook_id); 
  if (sys_irqsetpolicy( MOUSE_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &mouse_hook_id)) {
    printf("Error subscribing the mouse interrupts! \n");
    return 1;
  }
  return 0;
}

int mouse_unsubscribe_int() {
    
    if (sys_irqrmpolicy(&mouse_hook_id)) {
        printf("Error unsubscribing the mouse interrupts! \n");
        return 1;
    }

    return 0; 

}

int (timer_subscribe_int)(uint8_t *bit_no) {

  *bit_no = BIT(timer_hook_id);

  if (sys_irqsetpolicy(TIMER0_IRQ, IRQ_REENABLE, &timer_hook_id)) {
    printf("Error setting the policy! \n");
    return 1;
  }

  return 0;

}

int (timer_unsubscribe_int)() {
  
  if (sys_irqrmpolicy(&timer_hook_id)) {
    printf("Error removing the policy! \n");
    return 1;
  }

  return 0;
}

int mouse_write_cmd(int port, uint8_t cmd) {

  int timecounter = 0;

  do {

    if (mouse_read_status(&status)) {
      return 1;
    }

  
    if ((status & IBF) == 0) {
      if (sys_outb(port, cmd) != OK) {
        printf("Error writing the command");
        return 1;
      }
      return 0;
    }

  } while (timecounter < 10);

  return 1;

}

int mouse_read_status(uint8_t *status){

  if (util_sys_inb(STATUS_REG, status)) {
    printf("Error reading the mouse status");
    return 1;
  }

  return 0;
}

int mouse_read_data(uint8_t *data) {

  if (mouse_read_status(&status)) return 1;

    if (util_sys_inb(OUT_BUF, data)) {
      printf("Error reading the buffer! \n");
      return 1;
    }

    if (status & (TIMEOUT_ERR | PARITY_ERR)) {
      printf("Error: Invalid data! \n");
      return 1;
    }

    return 0;
  }


void timer_ih() {

  timer_counter++;

}

int mouse_command_handler(uint8_t cmd) {

  int count = 0;
  uint8_t asw = 0;

  while (count != 3) {

  
    if (wait_ibf_clear()) {
      printf("Error waiting for IBF to clear! \n");
      return 1;
    }
    if (mouse_write_cmd(STATUS_REG, WRITE_BYTE_MOUSE)) {
      printf("Error writing 0xD4 to port 0x64! \n");
      return 1;
    }


    if (wait_ibf_clear()) {
      printf("Error waiting for IBF to clear! \n");
      return 1;
    }
    if (mouse_write_cmd(IN_BUF_ARG, cmd)) {
      printf("Error writing the command to port 0x60! \n");
      return 1;
    }

    if(wait_obf_full()) {
      printf("Error waiting for OBF to fill! \n");
      return 1;
    }

    if (mouse_read_data(&asw)) {
      printf("Error reading the data! \n");
      return 1;
    }

    if (asw == ACK) {
      return 0;
    }
    
    count++;
  
  }

  printf("Error: Couldn't write the command after 3 tries! \n");
  return 1;

}


int wait_ibf_clear() {

  int counter = 10; 
  
  while (counter--) {

    if (mouse_read_status(&status))
      continue;

    if (!(status & IBF)) //Se está vazio, então passa
      return 0;
    

  }

  return 1;
}


int wait_obf_full() {

  int counter = 10; 

  while(counter--) {

    if (mouse_read_status(&status)) 
      continue;

    if (status & OBF) //se tem algo para ler, então passa
      return 0;
    
  }

  printf("Error: Couldn't read the data after 10 tries! \n");
  return 1;
}


void build_packet(struct packet *pp) {

  pp->lb = pp->bytes[0] & LB;
  pp->rb = pp->bytes[0] & RB;
  pp->mb = pp->bytes[0] & MB;
  pp->delta_x = pp->bytes[0] & X_MSB ? pp->bytes[1] | 0xff00 : pp->bytes[1];
  pp->delta_y = pp->bytes[0] & Y_MSB ? pp->bytes[2] | 0xff00 : pp->bytes[2];
  pp->x_ov = pp->bytes[0] & X_OV ;
  pp->y_ov = pp->bytes[0] & Y_OV;

}
