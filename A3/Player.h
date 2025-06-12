#ifndef __PLAYER__
#define __PLAYER__

#include <allegro5/allegro5.h>														//Biblioteca base do Allegro
#include "Joystick.h"
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
struct player *create_player ();

// struct player *move_player ();

// struct player *player_shot ();

void destroy_player (struct player *p);

#endif
