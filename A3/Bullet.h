#ifndef __BULLET__
#define __BULLET__

#include <allegro5/allegro5.h>  //Biblioteca base do Allegro
#include "defines.h"



struct bullet {
    long x, y;
    unsigned char trajectory;
    struct bullet *next;

    ALLEGRO_BITMAP *sprite;     // Foto da bala (vai ser carregada só uma vez lá dentro do jogo)
};

// x e y: definem a coordenada aonde a bullet, spawna
struct bullet *bullet_create (long x, long y, unsigned char trajectory, struct bullet *next, ALLEGRO_BITMAP *sprite);

void bullet_move(struct bullet **head, ALLEGRO_BITMAP* bullet);

void bullet_destroy (struct bullet *b);


#endif