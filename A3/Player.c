// Mac path: /Users/sebas/Documents/Prog2/A3
// Linux DSBD path : /home/dsbd/prog2/A3

#include <stdio.h>
#include <stdlib.h>

#include "Player.h"


void move_enemy(struct player *enemy, struct player *p, int *state, float *timer) {
  if (!enemy || !enemy->active || !enemy->gun || !p || !p->active) return;

  int speed = ENEMY_VELOCITY; // Velocidade do inimigo


  // Atualiza o timer
  *timer -= 1.0 / 60.0; // Supondo que o jogo roda a 60 FPS

  // Alterna entre os estados a cada 1 segundo
  if (*timer <= 0) {
    check_collision (enemy, p);

    *state = (*state == 0) ? 1 : 0; // Alterna entre andar (0) e atirar (1)
    *timer = 1.0; // Reseta o timer para 1 segundo
  }

  // Comportamento baseado no estado
  if (*state == 0) { // Estado: andar
    if (enemy->x > p->x) { // Inimigo está à direita e deve ir para a esquerda
      enemy->x -= speed;
      enemy->joystick->right = 0;
      enemy->joystick->left = 1;
    } else {
      enemy->x += speed; // Move o inimigo na direção do jogador
      enemy->joystick->right = 1;
      enemy->joystick->left = 0;
    }      
    enemy->joystick->fire = 0; // Não atira enquanto anda
  } 
  else if (*state == 1) { // Estado: atirar
    enemy->joystick->fire = 1; // Atira
    if (enemy->joystick->fire && !enemy->gun->timer) {
        enemy->gun->shots = player_shot(enemy); // Cria uma bala
        enemy->gun->timer = GUN_COOLDOWN + 10;  // Define o cooldown da arma
    }
  }

  // Atualiza o cooldown da arma
  if (enemy->gun->timer) {
      enemy->gun->timer--;
  }
}

void move_player (struct joystick *element, ALLEGRO_EVENT event) {
  if (!element) {
    perror ("Não consegui mover player\n");
    return;
  }

  // Eventos de teclado %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

  if (event.type == ALLEGRO_EVENT_KEY_DOWN) {       // Presiona a telca
    
    // Ativa W A S D ou RUN ou FIRE
    switch (event.keyboard.keycode) {
      case ALLEGRO_KEY_W: element->up = 1; break;
      case ALLEGRO_KEY_A: element->left = 1; break;
      case ALLEGRO_KEY_S: element->down = 1; break;
      case ALLEGRO_KEY_D: element->right = 1; break;
      case ALLEGRO_KEY_LSHIFT: element->run = 1; break; // Corre!
      case ALLEGRO_KEY_SPACE: element->fire = 1; break;
    }
  }

  if (event.type == ALLEGRO_EVENT_KEY_UP) {       // Solta a tecla

    // Desativa W A S D ou RUN ou FIRE
    switch (event.keyboard.keycode) {         
      case ALLEGRO_KEY_W: element->up = 0; break;
      case ALLEGRO_KEY_A: element->left = 0; break;
      case ALLEGRO_KEY_S: element->down = 0; break;
      case ALLEGRO_KEY_D: element->right = 0; break;
      case ALLEGRO_KEY_LSHIFT: element->run = 0; break; // Corre!
      case ALLEGRO_KEY_SPACE: element->fire = 0; break;
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



void can_move (struct player *p) {
  if (!p || !p->active) return;

  float vel = p->vel;

  if (p->joystick->run)
    vel = vel * 2;


  if (p->joystick->right && p->x < RIGHT_LIMIT) {
    p->x += vel;
    p->last_direction = 0;
  }
  if (p->joystick->left && p->x > LEFT_LIMIT) {
    p->x -= vel;
    p->last_direction = 1;
  }

  // Não deixa passar do chão
  if (p->joystick->down && p->y_size < FLOOR_LIMIT) {
    p->y += vel;
  }

  if (p->joystick->fire && !p->gun->timer) {
    if (!p->gun->timer) {
      p->gun->shots = player_shot (p);
      p->gun->timer = GUN_COOLDOWN;
    }
  }

  if (p->gun->timer)
    p->gun->timer--;

}

// Confere ver a colisão entre a bala e o alvo 
void check_collision (struct player *shooter, struct player *target) {
  if (!shooter || !shooter->active || !shooter->gun || !target) return;

  struct bullet *prev = NULL;
  struct bullet *shot = shooter->gun->shots;


  while (shot != NULL) {
    // Testa se o shot está dentro da hitbox do target (se acertou o tiro ou não)
    if (((shot->x >= target->x) && (shot->x <= target->x + target->x_size)) &&    // Se a bala está entre o espaço em X
        ((shot->y >= target->y) && (shot->y <= target->y + target->y_size)))      // E TAMBÉM está entre o espaço em Y
    {
      printf ("ACERTOU O TIRO !!!!!!!!\n");
        target->life -= SHOT_DAMAGE; // Reduz a vida do alvo

        // Remove a bala da lista
        if (prev) { // Se não for a primeira bala da lista
            prev->next = shot->next;
        } else { // Se for a primeira bala da lista
            shooter->gun->shots = shot->next;
        }

        struct bullet *to_destroy = shot;
        shot = shot->next; // Avança para o próximo elemento
        bullet_destroy(to_destroy); // Destrói a bala atual
        continue; // Continua verificando as próximas balas
    }

    // Avança para o próximo elemento
    prev = shot;
    shot = shot->next;
  }
}

int check_kill(struct player *p, struct player *enemy) {
  if (!p || !enemy || !enemy->active) return -1;

  if (p->life <= 0) return 1;
  if (enemy->life <= 0) return 2;
  
  return 0;
}


struct player *player_create (float x, float y, long life, ALLEGRO_BITMAP *sprite, ALLEGRO_BITMAP *bullet) {
  struct player* p = malloc (sizeof (struct player));
  if (!p) {
    perror ("Erro no malloc do create_player\n");
    return NULL;
  }

  // Posição inicial do player
  p->x = x;
  p->y = y;

  p->x_size = 220.0;       // largura da hitbox
  p->y_size = 262.5;       // altura da hitbox


  // Status de vida e velocidade do player
  p->life = life;
  p->vel = 5.0;
  p->last_direction = 0; // 0: Direita, 1: Esquerda

  p->active = 1;  // 0: foi destruído

  p->joystick = create_joystick (bullet);
  if (!p->joystick) {
    perror ("Erro ao criar o Joystick\n");
    return NULL;
  }

  p->gun = pistol_create (bullet);
  if (!p->gun) {
    perror ("Erro ao criar a Gun\n");
    return NULL;
  }

  // Crio a sprite do meu jogador
  p->sprite = sprite;

  return p;
}


struct bullet *player_shot (struct player *p) {

  if (!p) {
          fprintf(stderr, "Erro: ponteiro do jogador é NULL\n");
          return NULL;
      }
      if (!p->active) {
          fprintf(stderr, "Erro: jogador está inativo\n");
          return NULL;
      }
      if (!p->gun) {
          fprintf(stderr, "Erro: arma do jogador é NULL\n");
          return NULL;
      }
      if (!p->joystick) {
          fprintf(stderr, "Erro: joystick do jogador é NULL\n");
          return NULL;
      }
      if (!p->gun->bullet_sprite) {
          fprintf(stderr, "Erro: sprite da bala não carregada\n");
  }

  // if (!p || !p->active || !p->gun || !p->joystick || !p->gun->bullet_sprite) {
  //   perror ("Erro na função player_shot\n");
  //   return NULL;
  // }



  // Nova bala, será a nova cabeça da lista
  struct bullet *shot_head;
  int dir = 0;


  if (p->joystick->left || p->joystick->right)
    direction (p, &dir);
  else dir = p->last_direction;

  shot_head = pistol_shot (p->x, p->y, dir, p->gun, p->gun->bullet_sprite);
  if (!shot_head) {
    perror ("Erro ao criar a shot_head\n");
    return NULL;
  }

  // Se criou a nova cabeça, coloca ela na arma
  return shot_head;

}


void destroy_player(struct player *p) {
  if (!p) return;

  // Marca como inativo
  p->active = 0;

  // Destrói a pistola (incluindo balas)
  if (p->gun) {
      pistol_destroy(p->gun);
      p->gun = NULL;
  }

  // Destrói o joystick
  if (p->joystick) {
      destroy_joystick(p->joystick);
      p->joystick = NULL;
  }


  free(p);
  p = NULL;
}

int wich_sprite (struct player *p) {

	if (!p || !p->active || !p->sprite || !p->joystick) {
		fprintf(stderr, "Erro: ponteiro nulo no witch_sprite\n");
		return -1;
	}

	// Parado
	int num_sprite = 0;

	if (p->joystick->right && p->joystick->left) {
		num_sprite = 0;
	} else if (p->joystick->run) {
		num_sprite = 1;	// Rungun
	} else if (p->joystick->up && !p->joystick->fire) {
		num_sprite = 4;	// Jump
	} else if (p->joystick->down) {
		num_sprite = 5;	// Downgun
	} else if (p->joystick->fire && !p->joystick->up ) {
		num_sprite = 3;	// Rungun
	} else if (p->joystick->left && !p->joystick->up && !p->joystick->fire) {
		num_sprite = 1;	// Run1
	} else if (p->joystick->right && !p->joystick->up && !p->joystick->fire) {
		num_sprite = 1;	// Run2
	} else if (p->joystick->up && p->joystick->fire) {
		num_sprite = 6;	// Jumpgun
	} else if (p->joystick->down && p->joystick->fire) {
		num_sprite = 5;	// Downgun
	} else {
		num_sprite = 0;	// Stand
	}

	return num_sprite;

} 

int witch_virus (struct player *p) {
  int num_sprite = 2;

  if (p->joystick->right && p->joystick->left) {
    num_sprite = 2; // Stand
  } else if (p->joystick->right) {
    num_sprite = 1; // Direita
  } else if (p->joystick->left) {
    num_sprite = 0; // Esquerda
  }

  return num_sprite;
}


void direction (struct player *p, int *dir) {
	if (!p || !p->active || !p->joystick) {
		fprintf(stderr, "Erro: ponteiro nulo no direction\n");
		return;
	}

  if (p->joystick->right && p->joystick->left)
    return;

  *dir = p->joystick->left;   // 0: right, 1: left

}