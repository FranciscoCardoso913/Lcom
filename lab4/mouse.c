#include "mouse.h"
#include <lcom/lcf.h>
#include "../lab3/i8042.h"

#include <stdint.h>

extern int mouse_hook_id;
extern int counter;
struct packet pp;
bool wait_ibf_empty() {
  int tries = 10;
  uint8_t status;
  while (tries--) {
    if (util_sys_inb(STAT_REG,&status)) {
      continue;
    }

    if (!(status & KBC_ST_IBF))
      return true;
  }

  printf("Timed out in wait_ibf_empty\n");
  return false;
}

bool (enable_stream_mode)(){
    uint8_t mouse_response=0;
    int tries=5;
    while(mouse_response!=0xFA && tries){
        if(!wait_ibf_empty())return 1;
        sys_outb(0x64,0xD4);
        if(!wait_ibf_empty())return 1;
        sys_outb(0x60,0xF4);
        util_sys_inb(OUT_BUF, &mouse_response);
        tries--;
    }    
    return 0;
}
bool (disable_stream_mode)(){
    int tries=5;
     uint8_t mouse_response=0;
    while(mouse_response!=0xFA && tries){
        if(!wait_ibf_empty())return 1;
        sys_outb(0x64,0xD4);
        if(!wait_ibf_empty())return 1;
        sys_outb(0x60,0xF5);
        util_sys_inb(OUT_BUF, &mouse_response);
        tries--;
    } 
    return 0;
}

int (mouse_subscribe_int)(uint8_t *bit_no){
   *bit_no=mouse_hook_id;
   if(sys_irqsetpolicy(MOUSE_IRQ,IRQ_REENABLE|IRQ_EXCLUSIVE,&mouse_hook_id)){
    printf("Error while subscribing.");
    return 1;
   }
   return 0;
};

int (mouse_unsubscribe_int)(){
    if(sys_irqrmpolicy(&mouse_hook_id)){
        printf("Error while unsubscribing");
        return 1;
    }
    return 0;
};
void (mouse_ih)(){
    uint8_t byte;
    uint8_t status;
    if(util_sys_inb(STAT_REG,&status)){
        printf("Error while making the request to read");

    }
    if (status & (KBC_ST_OBF | BIT(5))) {

    util_sys_inb(OUT_BUF, &byte);

    if (status & (KBC_PAR_ERROR | KBC_TO_ERROR)) {
      printf("Parity or timeout error when reading mouse data\n");
      return;
    }

    if (counter == 0 && !(byte & BIT(3))) {
      printf("Mouse wasn't synched\n");
      return;
    }
    pp.bytes[counter++] = byte;
  }
}

struct packet (mouse_return_packet)() {
  pp.lb = pp.bytes[0] & BIT(0);
  pp.rb = pp.bytes[0] & BIT(1);
  pp.mb = pp.bytes[0] & BIT(2);
  pp.delta_x = pp.bytes[0] & BIT(4) ? pp.bytes[1] | 0xff00 : pp.bytes[1];
  pp.delta_y = pp.bytes[0] & BIT(5) ? pp.bytes[2] | 0xff00 : pp.bytes[2];
  pp.x_ov = pp.bytes[0] & BIT(6);
  pp.y_ov = pp.bytes[0] & BIT(7);
  return pp;
}


   
