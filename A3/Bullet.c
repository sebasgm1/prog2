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

// Bullet Move já vai dando free em quem saiu do display
// void bullet_move (struct bullet *b) {

//     if (!b) {
//         perror ("Erro ao mover a Bullet\n");
//         return;
//     }

//     struct bullet *ant = b;

//     // faz a verificação caso tenha apenas uma bala na lista
//     // if (!b->next) {
//     //     switch (b->trajectory) {
//     //         case 1: // indo pra direita
//     //             if (b->x < 540)
//     //                 bullet_destroy (b);
//     //             else
//     //                 b->x += BULLET_SPEED;
//     //             break;
//     //         case 0:
//     //             if (b->x > 0)
//     //                 bullet_destroy (b);
//     //             else
//     //                 b->x -= BULLET_SPEED;
//     //             break;
//     //     }
//     // }
//     // else {
    
//         // Se tiver mais de uma bala, cai nesse for
//         for (struct bullet *i = b; i != NULL; i = (struct bullet*) i->next) {

//             switch (i->trajectory) {
//                 case 1:   // indo pra direita
//                     if (i->x < 540) {
//                         ant->next = i->next;
//                         if (i != b)
//                             bullet_destroy (i);
//                         else {  // atualizo a cabeça da lista
//                             ant = b;
//                             b = b->next;
//                             bullet_destroy (ant);
//                         }
//                     } 
//                     else
//                         i->x += BULLET_SPEED; 
//                     break;

//                 case 0:   // indo pra esquerda
//                     if (i->x > 0) {
//                         ant->next = i->next;
//                         if (i != b)
//                             bullet_destroy (i);
//                         else {  // atualizo a cabeça da lista
//                             ant = b;
//                             b = b->next;
//                             bullet_destroy (ant);
//                         }
//                     }
//                     else
//                         i->x -= BULLET_SPEED; 
//                     break;
//             }
//             ant = i;
//         }
//     // }    


// }



void bullet_destroy (struct bullet *b) {

    if (!b) return;
    free (b);
    b = NULL;
}


