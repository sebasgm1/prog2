#ifndef __PISTOL__
#define __PISTOL__

#include "Bullet.h"

#define SHOT_DELAY 10   // A cada segundo eu tiro 6 vezes

struct pistol {
    unsigned char timer;   // Delay da lista
    struct bullet *shots;  // Cabeça da lista
};

struct pistol *pistol_create ();

struct bullet* pistol_shot(unsigned short x, unsigned short y, unsigned char trajectory, struct pistol *gun, ALLEGRO_BITMAP *bullet_sprite);

void pistol_destroy (struct pistol *p);

#endif