#include <lcom/lcf.h>

#include <stdbool.h>
#include <stdint.h>

#include "game.h"
#include "maps/map.h"
#include "count_down/count_down.h"
int frames_per_second=60;
int current_frame=0;
extern Map* map1;
uint8_t irq_mouse, irq_timer, irq_keyboard;

int game_init() {
    if (video_mode()) {
        printf("Error setting video mode.\n");
        return 1;
    }

    if (timer_subscribe_int(&irq_timer)) {
        printf("Error subscribing timer interrupts.\n");
        return 1;
    }

    timer_set_frequency(0, 60);

    if (kbc_init(&irq_keyboard, &irq_mouse)) {
        printf("Error initializing kbc.\n");
        return 1;
    }

    if (load_sprites()) {
        printf("Error loading cursor.\n");
        return 1;
    }
      if(load_Maps()){
        return 1;
    }
   
    

    return 0;
}

int game_exit() {
    if (kbc_disable() != OK) {
        printf("Error disabling kbc.\n");
        return 1;
    }

    if (timer_unsubscribe_int()) {
        printf("Error unsubscribing timer interrupts.\n");
        return 1;
    }

    if (text_mode() != OK) {
        printf("Error exiting video mode.\n");
        return 1;
    }

    return 0;
}

game_state state = MAIN_MENU;

int game_loop() {
    int ipc_status, r;
    message msg;

    do {
        if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
            printf("Error receiving message: %d\n", r);
            continue;
        }

        if (is_ipc_notify(ipc_status)) {
            switch (_ENDPOINT_P(msg.m_source)) {
            case HARDWARE:
                if (msg.m_notify.interrupts & irq_mouse) {
                    mouse_ih();
                    if (!mouse_packet.complete) continue;

                    cursor->x = mouse_packet.x;
                    cursor->y = mouse_packet.y;
                }

                if (msg.m_notify.interrupts & irq_timer) {
                    timer_ih();
                    draw_screen();
                }

                if (msg.m_notify.interrupts & irq_keyboard) {
                    keyboard_ih();
                    if (!keyboard_packet.complete) continue;
                    keyboard_key k = keyboard_get_key();

                    switch (k) {
                    case KEY_ESC:
                        if (state == MAIN_MENU) state = EXIT;
                        else state = MAIN_MENU;
                        clear_background();
                        break;

                    case KEY_A: case KEY_W: case KEY_S: case KEY_D:
                        fireboy_move(k);
                        break;

                    case KEY_LEFT: case KEY_UP: case KEY_DOWN: case KEY_RIGHT:
                        watergirl_move(k);
                        break;

                    default:
                        break;
                    }
                }
            default:
                break;
            }
        }
    } while (state != EXIT);

    return 0;
}

int draw_screen() {
    switch (state) {
    case MAIN_MENU:
        draw_main_menu();
        break;
    case SETTINGS_MENU:
        draw_settings_menu();
        break;
    case GAME:
        draw_game();

        break;
    case PAUSE_MENU:
        draw_pause_menu();
        break;
    case GAME_OVER:
        draw_game_over();
        break;
    default:
        break;
    }

    return 0;
}

int draw_main_menu() {
    Sprite * chosen[3];
    draw_sprite(logo);

    if (mouse_over_sprite(play_text)) {
        chosen[0]=(play_texth);
        if (mouse_lclick_sprite(play_text)) {
            state = GAME;
            start_counter(120);
             clear_background();
             draw_map(map1);
             return 0;
        }
    }
    else {
        chosen[0]=(play_text);
    }
    if (mouse_over_sprite(settings_text)) {
        chosen[1]=(settings_texth);
        if (mouse_lclick_sprite(settings_text)) {
            state = SETTINGS_MENU;
        }
    }
    else {
        chosen[1]=(settings_text);
    }
    if (mouse_over_sprite(exit_text)) {
        chosen[2]=(exit_texth);
        if (mouse_lclick_sprite(exit_text)) {
            state = EXIT;
        }
    }
    else {
        chosen[2]=(exit_text);
    }
    draw_sprite(chosen[0]);
    draw_sprite(chosen[1]);
    draw_sprite(chosen[2]);
    draw_sprite(cursor);
    draw_buffer();
    erase_sprite(cursor);
    erase_sprite(logo);
    erase_sprite(chosen[0]);
    erase_sprite(chosen[1]);
    erase_sprite(chosen[2]);
    


    return 0;
}

int draw_settings_menu() {
    return 0;
}

int draw_game() {
    draw_sprite(cursor); 
    draw_buffer();
    erase_sprite(cursor);
     if((current_frame++)==frames_per_second){ 
        decrement_counter();
        current_frame=0;
    }
    draw_counter();

    return 0;
}

int draw_pause_menu() {
    return 0;
}

int draw_game_over() {
    return 0;
}

int fireboy_move(keyboard_key key) {
    switch (key) {
    case KEY_A:
        fireboy->x = fireboy->x - 5 > 5 ? fireboy->x - 5 : 5;
        break;
    case KEY_W:
        fireboy->y = fireboy->y - 5 > 5 ? fireboy->y - 5 : 5;
        break;
    case KEY_S:
        fireboy->y = fireboy->y + 5 < get_vres() - fireboy->height ? fireboy->y + 5 : get_vres() - fireboy->height;
        break;
    case KEY_D:
        fireboy->x = fireboy->x + 5 < get_hres() - fireboy->width ? fireboy->x + 5 : get_hres() - fireboy->width;
        break;
    default:
        break;
    }

    return 0;
}

int watergirl_move(keyboard_key key) {
    switch (key) {
    case KEY_LEFT:
        watergirl->x = watergirl->x - 5 > 5 ? watergirl->x - 5 : 5;
        break;
    case KEY_UP:
        watergirl->y = watergirl->y - 5 > 5 ? watergirl->y - 5 : 5;
        break;
    case KEY_DOWN:
        watergirl->y = watergirl->y + 5 < get_vres() - watergirl->height ? watergirl->y + 5 : get_vres() - watergirl->height;
        break;
    case KEY_RIGHT:
        watergirl->x = watergirl->x + 5 < get_hres() - watergirl->width ? watergirl->x + 5 : get_hres() - watergirl->width;
        break;
    default:
        break;
    }

    return 0;
}