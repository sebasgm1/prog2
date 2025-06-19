#include <stdio.h>
#include <stdlib.h>

#include "Bullet.h"


// Talvez adicionar um ponteiro pra sprite?
struct bullet *bullet_create (long x, long y, unsigned char trajectory, struct bullet *next, ALLEGRO_BITMAP *sprite) {

    struct bullet *b = malloc (sizeof (struct bullet));
    if (!b) return NULL;

    b->x = x;
    b->y = y;
    b->trajectory = trajectory;
    b->next = next;
    
    if (sprite)
        b->sprite = sprite;
    else {
        perror ("Parametro de sprite Bullet inválido\n");
        return NULL;
    }

    return b;
}

void bullet_move (struct bullet *b) {

    if (!b) {
        perror ("Erro ao mover a Bullet\n");
        return;
    }

    for (struct bullet *i = b; i != NULL; i = (struct bullet*) i->next) {
        switch (i->trajectory) {
            case 1: i->x += BULLET_SPEED; break;  // indo pra direita
            case 0: i->x -= BULLET_SPEED; break;  // indo pra esquerda
        }  
    }

}

void bullet_destroy (struct bullet *b) {

    if (!b) return;
    free (b);
    b = NULL;
}


