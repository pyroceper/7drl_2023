#ifndef ENTITY_H
#define ENTITY_H

struct Entity
{
    int x, y;
    bool is_movement_done;
    int health;
    float animation_tick;
    int animation_index;
    int is_left = 1;
    bool is_dead = false;
};


#endif