#include <stdio.h>

#include "Joystick.h"
#include "Player.h"

void move_player (joystick *element, ALLEGRO_EVENT event) {

  // Move W A S D
	if (event.keyboard.keycode == ALLEGRO_KEY_W) {
    element->up = 1;
	}
  else element->up = 0;

  if (event.keyboard.keycode == ALLEGRO_KEY_A) {
    element->left = 1;
  }
  else element->left = 0;

  if (event.keyboard.keycode == ALLEGRO_KEY_S) {
    element->down = 1;
  }
  else element->down = 0;

  if (event.keyboard.keycode == ALLEGRO_KEY_D) {
    element->right = 1;
  }
  else element->right = 0;



  // Decide se ele vai correr ou não
  if (event.keyboard.keycode == ALLEGRO_KEY_LSHIFT) {
    element->run = 1;
  }
  else element->run = 0;



  // Atirar
  if (event.mouse.button == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
    element->fire = 1;
  }
  else element->fire = 0;
}


struct player *create_player () {
  struct player* p = malloc (sizeof (struct player));
  if (!p) {
    perro ("Erro no malloc do create_player\n");
    exit (0);
  }

  // Posição inicial do player
  p->x = 30;
  p->y = 30;

  // Status de vida e velocidade do player
  p->life = 10;
  p->vel = 5;

  p->joystick = 

  // Crio a sprite do meu jogador
  p->sprite = al_load_bitmap ("/home/ibm/sgm24/prog2/A3/sprites/Psprite1.png");

  return p;
}


void destroy_player (struct player *p) {
  if (!p) {
    // ... outros frees

    free (p);
    p = NULL;
  }
}