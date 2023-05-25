#include "character.h"


int (create_characters)() {
    fireboy = (Character*)malloc(sizeof(Character));

    if (fireboy == NULL) return 1;
    fireboy->sprite = create_sprite((xpm_map_t)(fireL1_xpm), 600, 600, 0, 0);
    fireboy->direction = DEFAULT;
    fireboy->animation_delay = 5;
    fireboy->current_sprite = 0;
    fireboy->frames_to_next_change = 0;

    fireboy->left[0] = load_img((xpm_map_t)fireL1_xpm);
    fireboy->left[1] = load_img((xpm_map_t)fireL2_xpm);
    fireboy->left[2] = load_img((xpm_map_t)fireL3_xpm);
    fireboy->left[3] = load_img((xpm_map_t)fireL4_xpm);
    fireboy->left[4] = load_img((xpm_map_t)fireL5_xpm);
    fireboy->left[5] = load_img((xpm_map_t)fireL6_xpm);
    fireboy->right[0] = load_img((xpm_map_t)fireR1_xpm);
    fireboy->right[1] = load_img((xpm_map_t)fireR2_xpm);
    fireboy->right[2] = load_img((xpm_map_t)fireR3_xpm);
    fireboy->right[3] = load_img((xpm_map_t)fireR4_xpm);
    fireboy->right[4] = load_img((xpm_map_t)fireR5_xpm);
    fireboy->right[5] = load_img((xpm_map_t)fireR6_xpm);
    fireboy->front = load_img((xpm_map_t)fire_default_xpm);

    watergirl = (Character*)malloc(sizeof(Character));

    if (watergirl == NULL) return 1;
    watergirl->sprite = create_sprite((xpm_map_t)(waterL1_xpm), 600, 600, 0, 0);
    watergirl->direction = DEFAULT;
    watergirl->animation_delay = 5;
    watergirl->current_sprite = 0;
    watergirl->frames_to_next_change = 0;
    watergirl->left[0] = load_img((xpm_map_t)waterL1_xpm);
    watergirl->left[1] = load_img((xpm_map_t)waterL2_xpm);
    watergirl->left[2] = load_img((xpm_map_t)waterL3_xpm);
    watergirl->left[3] = load_img((xpm_map_t)waterL4_xpm);
    watergirl->left[4] = load_img((xpm_map_t)waterL5_xpm);
    watergirl->left[5] = load_img((xpm_map_t)waterL6_xpm);
    watergirl->right[0] = load_img((xpm_map_t)waterR1_xpm);
    watergirl->right[1] = load_img((xpm_map_t)waterR2_xpm);
    watergirl->right[2] = load_img((xpm_map_t)waterR3_xpm);
    watergirl->right[3] = load_img((xpm_map_t)waterR4_xpm);
    watergirl->right[4] = load_img((xpm_map_t)waterR5_xpm);
    watergirl->right[5] = load_img((xpm_map_t)waterR6_xpm);
    watergirl->front = load_img((xpm_map_t)water_default_xpm);

    return 0;
}

int (set_position)(Character* character, int x, int y) {
    character->sprite->x = x;
    character->sprite->y = y;
    return 0;
}

char get_tile(Map* map, u_int32_t x, u_int32_t y) {
    uint32_t index = y / TILE_SIZE * map->columns + x / TILE_SIZE;

    return map->map[index];
}

int wall_down(Character* character) {
    uint32_t x = character->sprite->x  ;
    uint32_t y = character->sprite->y + character->sprite->height+ character->sprite->yspeed;

    char tile = get_tile(map1, x + CHECKBOX_PADDING, y);

    if (tile == 'A') { return 1; }
    if(get_tile(map1,x + character->sprite->width - CHECKBOX_PADDING,y)=='A') return 1;

    return 0;
}

int wall_left(Character* character) {
    
    uint32_t x = character->sprite->x -character->sprite->xspeed;
    uint32_t y = character->sprite->y ;

    char tile = get_tile(map1, x, y + CHECKBOX_PADDING);

    if (tile == 'A') { return 1; }
    if(get_tile(map1,x,y+ character->sprite->height -CHECKBOX_PADDING)=='A') return 1;

    return 0;
}

int wall_right(Character* character) {
    uint32_t x = character->sprite->x + character->sprite->width + character->sprite->xspeed;
    uint32_t y = character->sprite->y ;

    char tile = get_tile(map1, x, y +CHECKBOX_PADDING);

    if (tile == 'A') { return 1; }
    if(get_tile(map1,x ,y+ character->sprite->height -CHECKBOX_PADDING)=='A') return 1;

    return 0;
}

int wall_up(Character* character) {
    uint32_t x = character->sprite->x ;
    uint32_t y = character->sprite->y -character->sprite->yspeed;

    char tile = get_tile(map1, x +CHECKBOX_PADDING, y);

    if (tile == 'A') { return 1; }
    if(get_tile(map1,x + character->sprite->width - CHECKBOX_PADDING,y)=='A') return 1;

    return 0;
}

int door_fire(Character* character) {
    uint32_t x = character->sprite->x + character->sprite->width / 2;
    uint32_t y = character->sprite->y + character->sprite->height / 2;

    char tile = get_tile(map1, x, y);

    if (tile == 'F') { return 1; }

    return 0;
}

int door_water(Character* character) {
    uint32_t x = character->sprite->x + character->sprite->width / 2;
    uint32_t y = character->sprite->y + character->sprite->height / 2;

    char tile = get_tile(map1, x, y);

    if (tile == 'W') { return 1; }

    return 0;
}

void update_character(Character* character) {
    if ( character->direction!=DEFAULT ) {
        if ((++character->frames_to_next_change) == character->animation_delay) {
            character->current_sprite++;
            character->frames_to_next_change = 0;
            if (character->current_sprite > 5) character->current_sprite = 0;
        }
    }

}
int (update_direction)(Character * character, enum Direction dir){
    if(character->direction!=dir){
         character->direction = dir;
        character->current_sprite = 0;
        character->frames_to_next_change = 0;
        return 1;
    }
    return 0;
}

void move(Character* character) {
    
    if (wall_down(character)) {
        /* Boy is on the ground, chill out! */
        character->sprite->yspeed = 0;
        while (!is_on_ground(character))
        {
            character->sprite->y++;
        }
        

        /* Put character on top of the ground */
        //character->sprite->y = (character->sprite->y / TILE_SIZE) * TILE_SIZE;
    }
    else {
        if (wall_up(character)) {
            character->sprite->y = (character->sprite->y / TILE_SIZE) * TILE_SIZE + TILE_SIZE;
            character->sprite->yspeed = 0;
        }

        character->sprite->y += character->sprite->yspeed;
        character->sprite->yspeed += GRAVITY;
    }

    /* Move to left or right */
    if (character->direction == LEFT && !wall_left(character)) {
        character->sprite->x -= character->sprite->xspeed;
    }
    else if (character->direction == RIGHT && !wall_right(character)) {
        character->sprite->x += character->sprite->xspeed;
    }
}

void (move_left)(Character* character) {
    //character->sprite->x -= 3 * character->sprite->xspeed;
    character->sprite->xspeed = DEFAULT_SPEED;
    update_direction(character, LEFT);

}
void (move_right)(Character* character) {
    //character->sprite->x += 3 * character->sprite->xspeed;
    character->sprite->xspeed = DEFAULT_SPEED;

    update_direction(character, RIGHT);
}

void (stop_moving)(Character* character) {
    character->sprite->xspeed = 0;
    character->direction = DEFAULT;
}

void (jump)(Character* character) {
    /* Get him off the ground or else move() will prevent jump! */

    if (is_on_ground(character)) {
        character->sprite->yspeed = -JUMP;
    }
}

void (character_current_sprite)(Character* character) {
    if (character->direction == LEFT) {
        character->sprite->map = character->left[character->current_sprite];

    }
    else if (character->direction == RIGHT) {
        character->sprite->map = character->right[character->current_sprite];
    }
    else {
        character->sprite->map = character->front;
    }
}

int(draw_character)(Character* Character) {
    character_current_sprite(Character);
    draw_sprite((Character->sprite));
    return 0;
}
int (is_on_ground)(Character * character){
    uint32_t x = character->sprite->x ;
    uint32_t y = character->sprite->y + character->sprite->height;

    char tile = get_tile(map1, x, y+1);

    if (tile == 'A') { return 1; }
    if(get_tile(map1,x + character->sprite->width,y)=='A') return 1;

    return 0;
}