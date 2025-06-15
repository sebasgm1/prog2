#ifndef __PLAYER__
#define __PLAYER__

#include <allegro5/allegro5.h>														//Biblioteca base do Allegro
#include "Joystick.h"



struct player {
  float x, y;               // Posição no mundo
  
  // Tamanho da hitbox
  float x_size;       
  float y_size;

  float vel;                // Velocidade dele
  long life;                // Vida
  struct joystick *joystick;

  ALLEGRO_BITMAP *sprite;   // Foto do jogador
};

// Cria o jogador e retorna o ponteiro
struct player *player_create ();

// Move o jogador de acordo com o evento
void move_player (struct joystick *element, ALLEGRO_EVENT event);

// Verifica se o jogador pode se mover
void can_move (struct player *p, int LimitY);

// struct player *player_shot ();

void destroy_player (struct player *p);

#endif
