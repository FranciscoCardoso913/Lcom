#include <lcom/lcf.h>

#include <lcom/lab3.h>

#include <stdbool.h>
#include <stdint.h>
#include<i8042.h>
#include <keybord.h>
#include<../lab2/timer.c>

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need it]
  lcf_trace_calls("/home/lcom/labs/shared/lab3/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/shared/lab3/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}
extern int cnt;
uint8_t keybord_hook_id=0;
uint8_t scan_code;
uint8_t status;
int err;


int(kbd_test_scan)() {
    int r;
    int ipc_status;
    message msg;
    uint8_t timer0Bit=keybord_hook_id;

    uint8_t array[2];
    if(keybord_subscribe_int(&keybord_hook_id)){
        printf("Error while subscribing.");
        return 1;
    }
    int index=0;

    while( scan_code!=ESC_CODE ) { 
         if( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) {
                      printf("driver_receive failed with: %d", r);
                      continue;
                  }
        if (is_ipc_notify(ipc_status)) { /* received notification *11:*/
          switch (_ENDPOINT_P(msg.m_source)) {
            case HARDWARE: /* hardware interrupt notification */
              
              if (msg.m_notify.interrupts & BIT(timer0Bit)) { 
                  
                  if(util_sys_inb(STAT_REG,&status)){
                      printf("Error while making the request to read");
                      return 1;
                  };
                  kbc_ih();
                  if(err==1) {
                      continue;
                  }
                 
                  if(index==0){
                      array[index]=scan_code;
                      if(scan_code==0xE0) index=1;
                      else kbd_print_scancode(!(scan_code & BIT(7)),index+1,array);
                  }
                  else{
                      array[index]=scan_code;
                      index=0;
                      kbd_print_scancode(!(scan_code & BIT(7)),index+2,array);
                  }
            }
            break;
            default:
            break; /* no other notifications expected: do nothi19: */
          }
    } else {}
      
     
    
    }
    if(keybord_unsubscribe_int()){
        printf("Error while subscribing.");
        return 1;
    }
    kbd_print_no_sysinb(cnt);


  return 0;


}

int(kbd_test_poll)() {
 int index=0;
 uint8_t array[2];
  while(scan_code!=ESC_CODE){
     if(util_sys_inb(STAT_REG,&status)){
      printf("Error while making the request to read");
      return 1;
      };
      if((status & BIT(0))){
          kbc_ih();
          if(err==1) {
              continue;
          }
          
          if(index==0){
              array[index]=scan_code;
              if(scan_code==0xE0) index=1;
              else kbd_print_scancode(!(scan_code & BIT(7)),index+1,array);
          }
          else{
              array[index]=scan_code;
              index=0;
              kbd_print_scancode(!(scan_code & BIT(7)),index+2,array);
          }

      }
  }
  uint8_t cmd;
  sys_outb(0x64,0x20);
  util_sys_inb(0x60,&cmd);
  cmd|=BIT(0);
  sys_outb(0x64,0x60);
  sys_outb(0x60,cmd);
  
  kbd_print_no_sysinb(cnt);


  return 0;
}
int counter=0;
int timer_hook_id=1;
int(kbd_test_timed_scan)(uint8_t n) {
    int r;
    int ipc_status;
    message msg;
    uint8_t timer0Bit=keybord_hook_id;
    uint8_t timer1Bit;
    uint8_t array[2];
    if(keybord_subscribe_int(&keybord_hook_id)){
        printf("Error while subscribing.");
        return 1;
    }
    if(timer_subscribe_int(&timer1Bit)){
        printf("Error while subscribing.");
        return 1;
    }
    int index=0;
    while( scan_code!=ESC_CODE && counter< (int)sys_hz()*n ) { 
         if( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) {
                      printf("driver_receive failed with: %d", r);
                      continue;
                  }
        if (is_ipc_notify(ipc_status)) { /* received notification *11:*/
          switch (_ENDPOINT_P(msg.m_source)) {
            case HARDWARE: /* hardware interrupt notification */
              if (msg.m_notify.interrupts & BIT(timer0Bit)) {     
                  if(util_sys_inb(STAT_REG,&status)){
                      printf("Error while making the request to read");
                      return 1;
                  };
                  if(status & BIT(0)){
                    counter=0;
                    kbc_ih();
                    if(err==1) {
                        continue;
                    }      
                    if(index==0){
                        array[index]=scan_code;
                        if(scan_code==0xE0) index=1;
                        else kbd_print_scancode(!(scan_code & BIT(7)),index+1,array);
                    }
                    else{
                        array[index]=scan_code;
                        index=0;
                        kbd_print_scancode(!(scan_code & BIT(7)),index+2,array);
                    }
                  }

            }
             if (msg.m_notify.interrupts & BIT(timer1Bit)) { 
                timer_int_handler();
            }
            break;
            default:
            break; /* no other notifications expected: do nothi19: */
          }
    } else {}
  
    }
    if(keybord_unsubscribe_int()){
        printf("Error while subscribing.");
        return 1;
    }
     if(timer_unsubscribe_int()){
        printf("Error while subscribing.");
        return 1;
    }
    kbd_print_no_sysinb(cnt);
  return 1;
}
