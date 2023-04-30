// IMPORTANT: you must include the following line in all your C files
#include <lcom/lcf.h>

#include <lcom/lab5.h>

#include <stdint.h>
#include <stdio.h>
#include <video.h>
#include <keyboard.h>
#include<../lab2/timer.c>

// Any header files included below this line should have been created by you

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need it]
  lcf_trace_calls("/home/lcom/labs/shared/lab5/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/shared/lab5/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}

int(video_test_init)(uint16_t mode, uint8_t delay) {

    if(video_set_mode(mode)){
        printf("Error while setting mode");
        return 1;
    }
    sleep(delay);
        if(vg_exit()){
            printf("Error while setting visual mode to text");
            return 1;
        }

  return 0;
}
uint8_t keyboard_hook_id=1;
uint8_t scan_code;
int err;
uint8_t status;
int(video_test_rectangle)(uint16_t mode, uint16_t x, uint16_t y,uint16_t width, uint16_t height, uint32_t color) {
  scan_code=0;
  uint8_t array[2];
  int index=0;
  uint8_t keyboard_irq_set=keyboard_hook_id;
  int r,ipc_status;

  message msg;
  if(init_vars(mode)) return 1;
   if(video_set_mode(mode)){
        printf("Error while setting mode");
        return 1;
    }
  if(keyboard_subscribe_int(&keyboard_irq_set)) return 1;
   if(video_draw_rectangle(x,y,width,height,color)) return 1;
   while( scan_code!=ESC_CODE ) { 
         if( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) {
                      printf("driver_receive failed with: %d", r);
                      continue;
                  }
        if (is_ipc_notify(ipc_status)) { /* received notification *11:*/
          switch (_ENDPOINT_P(msg.m_source)) {
            case HARDWARE: /* hardware interrupt notification */
              
              if (msg.m_notify.interrupts & BIT(keyboard_irq_set)) { 
                  
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
                  }
                  else{
                      array[index]=scan_code;
                      index=0;
                  }
            }
            break;
            default:
            break; /* no other notifications expected: do nothi19: */
          }
    } else {}
      
     
    
    }
    if(keyboard_unsubscribe_int()){
        printf("Error while subscribing.");
        return 1;
    } 
 
  if(vg_exit()){
      printf("Error while setting visual mode to text");
      return 1;
  }



  return 0;
}

int(video_test_pattern)(uint16_t mode, uint8_t no_rectangles, uint32_t first, uint8_t step) {
  /* To be completed */
   scan_code=0;
  uint8_t array[2];
  int index=0;
  uint8_t keyboard_irq_set=keyboard_hook_id;
  int r,ipc_status;

  message msg;
  if(init_vars(mode)) return 1;
   if(video_set_mode(mode)){
        printf("Error while setting mode");
        return 1;
    }
  if(keyboard_subscribe_int(&keyboard_irq_set)) return 1;
   if(video_draw_pattern(no_rectangles,first,step)) return 1;
   while( scan_code!=ESC_CODE ) { 
         if( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) {
                      printf("driver_receive failed with: %d", r);
                      continue;
                  }
        if (is_ipc_notify(ipc_status)) { /* received notification *11:*/
          switch (_ENDPOINT_P(msg.m_source)) {
            case HARDWARE: /* hardware interrupt notification */
              
              if (msg.m_notify.interrupts & BIT(keyboard_irq_set)) { 
                  
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
                  }
                  else{
                      array[index]=scan_code;
                      index=0;
                  }
            }
            break;
            default:
            break; /* no other notifications expected: do nothi19: */
          }
    } else {}
      
     
    
    }
    if(keyboard_unsubscribe_int()){
        printf("Error while subscribing.");
        return 1;
    } 
 
  if(vg_exit()){
      printf("Error while setting visual mode to text");
      return 1;
  }



  return 0;
}

int(video_test_xpm)(xpm_map_t xpm, uint16_t x, uint16_t y) {
     scan_code=0;
  uint8_t array[2];
  int index=0;
  uint8_t keyboard_irq_set=keyboard_hook_id;
  int r,ipc_status;
  message msg;
  if(init_vars(0x105)) return 1;
  if(video_set_mode(0x105))return 1;
   if(keyboard_subscribe_int(&keyboard_irq_set)) return 1;

   if( draw_xpm(xpm, x,y)) return 1;
   while( scan_code!=ESC_CODE ) { 
         if( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) {
                      printf("driver_receive failed with: %d", r);
                      continue;
                  }
        if (is_ipc_notify(ipc_status)) { /* received notification *11:*/
          switch (_ENDPOINT_P(msg.m_source)) {
            case HARDWARE: /* hardware interrupt notification */
              
              if (msg.m_notify.interrupts & BIT(keyboard_irq_set)) { 
                  
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
                  }
                  else{
                      array[index]=scan_code;
                      index=0;
                  }
            }
            break;
            default:
            break; /* no other notifications expected: do nothi19: */
          }
    } else {}
      
     
    
    }
    if(keyboard_unsubscribe_int()){
        printf("Error while subscribing.");
        return 1;
    } 
    if(vg_exit()) return 1;
 
 return 0;
}
int timer_hook_id=0;
int timer_counter=0;
int(video_test_move)(xpm_map_t xpm, uint16_t xi, uint16_t yi, uint16_t xf, uint16_t yf, int16_t speed, uint8_t fr_rate) {
  uint8_t keyboard_irq_set=keyboard_hook_id;
  uint8_t timer_irq_set= timer_hook_id;
  int r,ipc_status,index=0;
  uint8_t array[2];
  message msg;
  scan_code=0;
  int x=xi,y=yi;
  int stopedFrames;
  int isVertical=0;
  if(yi==yf) isVertical=0;
  else if(xi==xf) isVertical=1;
  else return 1;
  if(init_vars(0x105))return 1;
  if(video_set_mode(0x105)) return 1;
  if(keyboard_subscribe_int(&keyboard_irq_set)) return 1;
  if(timer_subscribe_int(&timer_irq_set)) return 1;
  if(timer_set_frequency(0,fr_rate)) return 1;
  if(draw_xpm(xpm,x,y)) return 1;
   while( scan_code!=ESC_CODE ) { 
         if( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) {
                      printf("driver_receive failed with: %d", r);
                      continue;
                  }
        if (is_ipc_notify(ipc_status)) { /* received notification *11:*/
          switch (_ENDPOINT_P(msg.m_source)) {
            case HARDWARE: /* hardware interrupt notification */
              
              if (msg.m_notify.interrupts & BIT(keyboard_irq_set)) { 
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
                  }
                  else{
                      array[index]=scan_code;
                      index=0;
                  }
            }
             if (msg.m_notify.interrupts & BIT(timer_irq_set)) { 
               
                if(x>=xf && y >=yf)continue;
               clear_screen(x,y);
                if(speed<0){

                    if(stopedFrames==-speed){
                      if(isVertical)y++;
                      else x++;
                      if( draw_xpm(xpm, x,y)) return 1;
                      stopedFrames=0;
                    }
                    else stopedFrames++;
                }
                else{
                  if(isVertical)y+=speed;
                  else x+=speed;
                  if(y>yf) y=yf;
                  if(x>xf) x=xf;
                  if( draw_xpm(xpm, x,y)) return 1;
                }

             }
            break;
            default:
            break; /* no other notifications expected: do nothi19: */
          }
    } else {}
    }

  if(timer_unsubscribe_int()) return 1;
  if(keyboard_unsubscribe_int()) return 1;
  if(vg_exit()) return 1;

  return 0;
}

int(video_test_controller)() {
  /* To be completed */
  printf("%s(): under construction\n", __func__);

  return 1;
}
