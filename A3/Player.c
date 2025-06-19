#include <stdio.h>
#include <stdlib.h>

#include "Joystick.h"
#include "Player.h"

#define FLOOR_LIMIT 220
#define LEFT_LIMIT 100
#define RIGHT_LIMIT 860

#define GRAVITY 1
#define Y_VELOCITY 15


void move_player (struct joystick *element, ALLEGRO_EVENT event) {
  if (!element) {
    perror ("Não consegui mover player\n");
    return;
  }

  // Eventos de teclado %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

  if (event.type == ALLEGRO_EVENT_KEY_DOWN) {       // Presiona a telca
    
    // Ativa W A S D ou RUN
    switch (event.keyboard.keycode) {
      case ALLEGRO_KEY_W: element->up = 1; break;
      case ALLEGRO_KEY_A: element->left = 1; break;
      case ALLEGRO_KEY_S: element->down = 1; break;
      case ALLEGRO_KEY_D: element->right = 1; break;
      case ALLEGRO_KEY_LSHIFT: element->run = 1; break; // Corre!
    }
  }

  if (event.type == ALLEGRO_EVENT_KEY_UP) {       // Solta a tecla

    // Desativa W A S D ou RUN
    switch (event.keyboard.keycode) {         
      case ALLEGRO_KEY_W: element->up = 0; break;
      case ALLEGRO_KEY_A: element->left = 0; break;
      case ALLEGRO_KEY_S: element->down = 0; break;
      case ALLEGRO_KEY_D: element->right = 0; break;
      case ALLEGRO_KEY_LSHIFT: element->run = 0; break; // Corre!
    }
  }

  // Eventos de mouse %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

  // ATIRA
  if (event.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {

    element->fire = 1;
  }

  // Para de atirar
  if (event.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP) {

    element->fire = 0;
  }

  

}

void can_move (struct player *p, int LimitY) {

  float vel = p->vel;

  if (p->joystick->run)
    vel = vel * 1.7;


  if (p->joystick->right) {
    p->x += vel;
  }
  if (p->joystick->left) {
    p->x -= vel;
  }

  

  // Não deixa passar do chão
  if (p->joystick->down && p->y_size < LimitY) {
    p->y += vel;
  }

}



struct player *player_create () {
  struct player* p = malloc (sizeof (struct player));
  if (!p) {
    perror ("Erro no malloc do create_player\n");
    return NULL;
  }

  // Posição inicial do player
  // p->x = 30.0;
  // p->y = 220.0;

  p->x = 30.0;
  p->y = 220.0;

  p->x_size = 220.0;       // largura da hitbox
  p->y_size = 262.5;       // altura da hitbox


  // Status de vida e velocidade do player
  p->life = 10;
  p->vel = 5.0;

  p->joystick = create_joystick ();
  if (!p->joystick) return NULL;

  // Crio a sprite do meu jogador
  p->sprite = al_load_bitmap ("/home/dsbd/prog2/A3/sprites/player440x525.png");

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