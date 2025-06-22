#ifndef __PISTOL__
#define __PISTOL__

#include "Bullet.h"



struct pistol {
    unsigned char timer;   // Delay da lista

    ALLEGRO_BITMAP *bullet_sprite;
    struct bullet *shots;  // Cabeça da lista
};

struct pistol *pistol_create ();

struct bullet* pistol_shot(long x, long y, unsigned char trajectory, struct pistol *gun, ALLEGRO_BITMAP *bullet_sprite);

void pistol_destroy (struct pistol *p);

#endif