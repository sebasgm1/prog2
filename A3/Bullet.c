#include <stdio.h>
#include <stdlib.h>

#include "Bullet.h"


// Talvez adicionar um ponteiro pra sprite?
struct bullet *bullet_create (long x, long y, unsigned char trajectory, struct bullet *next, ALLEGRO_BITMAP *sprite) {

    struct bullet *b = malloc (sizeof (struct bullet));
    if (!b) return NULL;
    
    if (!trajectory) { // 0: direita
        b->x = x + 180;
    } else {
        b->x = x - 50;
    }
    b->y = y + 80;

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


// Bullet Move já vai dando free em quem saiu do display
void bullet_move(struct bullet **head, ALLEGRO_BITMAP* bullet) {
    if (!head || !bullet) return;

    struct bullet *curr = *head;
    struct bullet *prev = NULL;

    while (curr) {
        if (curr->x >= 960 || curr->x <= -60) { // Saiu dos limites
            struct bullet *to_remove = curr;
            curr = curr->next;
            
            if (prev) {
                prev->next = curr;
            } else {
                *head = curr;
            }
            
            bullet_destroy(to_remove);
            continue;
        }

        // Movimento normal
        curr->x += (curr->trajectory == 0) ? BULLET_SPEED : -BULLET_SPEED;
        
        // Desenho
        al_draw_scaled_bitmap(
            bullet,
            0, 0,
            al_get_bitmap_width(bullet), al_get_bitmap_height(bullet),
            curr->x, curr->y,
            al_get_bitmap_width(bullet) * 0.5, al_get_bitmap_height(bullet) * 0.5,
            curr->trajectory ? ALLEGRO_FLIP_HORIZONTAL : 0
        );

        prev = curr;
        curr = curr->next;
    }
}


void bullet_destroy (struct bullet *b) {

    if (!b) return;
    free (b);
    b = NULL;
}


