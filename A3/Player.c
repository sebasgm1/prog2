#include <stdio.h>
#include <stdlib.h>

#include "Joystick.h"
#include "Player.h"


void move_player (ALLEGRO_KEYBOARD_STATE *e, struct player *p,int Ylimit) {
  if (al_key_down(e, ALLEGRO_KEY_W)) p->y -= p->vel;

  if (al_key_down(e, ALLEGRO_KEY_A)) p->x -= p->vel;

  if (al_key_down(e, ALLEGRO_KEY_S) && p->y < Ylimit) p->y += p->vel;

  if (al_key_down(e, ALLEGRO_KEY_D)) p->x += p->vel;
} 


// void move_player (struct joystick *element, ALLEGRO_EVENT event) {

//   // Move W A S D
// 	if (event.keyboard.keycode == ALLEGRO_KEY_W) {
//     element->up = 1;
// 	}
//   else element->up = 0;

//   if (event.keyboard.keycode == ALLEGRO_KEY_A) {
//     element->left = 1;
//   }
//   else element->left = 0;

//   if (event.keyboard.keycode == ALLEGRO_KEY_S) {
//     element->down = 1;
//   }
//   else element->down = 0;

//   if (event.keyboard.keycode == ALLEGRO_KEY_D) {
//     element->right = 1;
//   }
//   else element->right = 0;


//   // Decide se ele vai correr ou não
//   if (event.keyboard.keycode == ALLEGRO_KEY_LSHIFT) {
//     element->run = 1;
//   }
//   else element->run = 0;


//   // Atirar!
//   if (event.mouse.button == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
//     element->fire = 1;
//   }
//   else element->fire = 0;

// }

void can_move (struct player *p, int LimitY) {

  if (p->joystick->right) {
    p->x += p->vel;
  }
  if (p->joystick->left) {
    p->x -= p->vel;
  }

  if (p->joystick->up) {
    p->y -= p->vel;
  }

  // Não deixa passar do chão
  if (p->joystick->down && p->y < LimitY) {
    p->y += p->vel;
  }

}



struct player *player_create () {
  struct player* p = malloc (sizeof (struct player));
  if (!p) {
    perror ("Erro no malloc do create_player\n");
    exit (0);
  }

  // Posição inicial do player
  p->x = 30.0;
  p->y = 220.0;

  // Status de vida e velocidade do player
  p->life = 10;
  p->vel = 2.0;

  p->joystick = create_joystick ();

  // Crio a sprite do meu jogador
  p->sprite = al_load_bitmap ("/home/dsbd/prog2/A3/sprites/Psprite1.png");

  return p;
}


void destroy_player (struct player *p) {
  if (!p) {
    destroy_joystick (p->joystick);
    al_destroy_bitmap (p->sprite);

    free (p);
    p = NULL;
  }
}