#ifndef __PLAYER__
#define __PLAYER__

#include <allegro5/allegro5.h>														//Biblioteca base do Allegro
#include "Joystick.h"
#include "Pistol.h"


struct player {
  float x, y;               // Posição no mundo
  
  // Tamanho da hitbox
  float x_size;       
  float y_size;

  float vel;                // Velocidade dele
  long life;                // Vida

  int last_direction;
  int active;

  struct joystick *joystick;
  struct pistol *gun;

  ALLEGRO_BITMAP *sprite;   // Foto do jogador
};


void move_enemy(struct player *enemy, struct player *p, int *state, float *timer);


// Verifica se o jogador pode se mover
void can_move (struct player *p);

// Verifica se um projétil acertou o target
void check_collision (struct player *shooter, struct player *target);

// Verifica a vida de cada um dos dois e retorna um número avisando qual deles morreu
int check_kill (struct player *p, struct player *enemy);

// Cria o jogador e retorna o ponteiro
struct player *player_create (float x, float y, long life, ALLEGRO_BITMAP *sprite);

// Move o jogador de acordo com o evento
void move_player (struct joystick *element, ALLEGRO_EVENT event);

// void update_bullets(struct player *player);

// Cria uma nova bala dentro da lista de balas
struct bullet *player_shot (struct player *p);

void destroy_player (struct player **p);

int wich_sprite (struct player *p);

void direction (struct player *p, int *dir);

#endif
