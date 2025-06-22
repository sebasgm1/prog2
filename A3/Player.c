// Mac path: /Users/sebas/Documents/Prog2/A3
// Linux DSBD path : /home/dsbd/prog2/A3

#include <stdio.h>
#include <stdlib.h>

#include "Player.h"



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

void update_bullets(struct player *player){																																										//Implementação da função que atualiza o posicionamento de projéteis conforme o movimento dos mesmos
	
	struct bullet *previous = NULL;																																												//Variável auxiliar para salvar a posição imediatamente anterior na fila
	for (struct bullet *index = player->gun->shots; index != NULL;){																																				//Para cada projétil presente na lista de projéteis disparados
		if (!index->trajectory) index->x -= BULLET_SPEED;																																					//Se a trajetória for para a esquerda, atualiza a posição para a esquerda
		else if (index->trajectory == 1) index->x += BULLET_SPEED;																																			//Se a trajetória for para a direita, atualiza a posição para a esquerda
		
		if ((index->x < 0) || (index->x > 960)){																																						//Verifica se o projétil saiu das bordas da janela
			if (previous){																																													//Verifica se não é o primeiro elemento da lista de projéteis
				previous->next = index->next;																																								//Se não for, salva o próximo projétil
				bullet_destroy(index);																																										//Chama o destrutor para o projétil atual
				index = previous->next;																																							//Atualiza para o próximo projétil
			}
			else {																																															//Se for o primeiro projétil da lista
				player->gun->shots = index->next;																																					//Atualiza o projétil no início da lista
				bullet_destroy(index);																																										//Chama o destrutor para o projétil atual
				index = player->gun->shots;																																									//Atualiza para o próximo projétil
			}
		}
		else{																																																//Se não saiu da tela
			previous = index;																																												//Atualiza o projétil anterior (para a próxima iteração)
			index = index->next;																																									//Atualiza para o próximo projétil
		}
	}
}


void can_move (struct player *p) {

  float vel = p->vel;

  if (p->joystick->run)
    vel = vel * 1.7;


  if (p->joystick->right && p->x < RIGHT_LIMIT) {
    p->x += vel;
  }
  if (p->joystick->left && p->x > LEFT_LIMIT) {
    p->x -= vel;
  }

  // Não deixa passar do chão
  if (p->joystick->down && p->y_size < FLOOR_LIMIT) {
    p->y += vel;
  }

  if (p->joystick->fire) {
    if (!p->gun->timer) {
      player_shot (p);
      p->gun->timer = GUN_COOLDOW;
    }
  }

  update_bullets (p);

}

// tenta ver a colisão entre a bala e o alvo
void check_collision (struct player *shooter, struct player *target) {
  if (!shooter || !target) return;


  for (struct bullet *shot = shooter->gun->shots; shot != NULL; shot->next)
    if (shot->trajectory) {   // Tiro indo pra direita
      if ((shot->x >= target->x) && (target->x + target->x_size <= ))
    }
    else { // Tiro indo pra esquerda

    }

}

int check_kill (struct player *p, struct player *enemy) {
  if (!p || !enemy) return -1;

  if (p->life == 0)   // Player morreu! return 1
    return 1;
  else if (enemy->life == 0)    // enemy morreu! return 2
    return 2;

  if (p->life == 0 && enemy->life == 0)   // EMPATE! matou e morreu
    return 0;
  
  return 3; // Tudo ok, ninguém morreu ainda
}


struct player *player_create (float x, float y, long life) {
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

  p->joystick = create_joystick ();
  if (!p->joystick) {
    perror ("Erro ao criar o Joystick\n");
    return NULL;
  }

  p->gun = pistol_create ();
  if (!p->gun) {
    perror ("Erro ao criar a Gun\n");
    return NULL;
  }

  // Crio a sprite do meu jogador
  p->sprite = al_load_bitmap ("/Users/sebas/Documents/Prog2/A3/sprites/player440x525.png");

  return p;
}


struct player *player_shot (struct player *p) {

  if (!p || p->gun) {
    perror ("Erro na função player_shot\n");
    return NULL;
  }

  // Nova bala, será a nova cabeça da lista
  struct bullet *shot_head;
  int dir = direction (p);  // 0: direita; 1: esquerda

  shot_head = pistol_shot (p->x, p->y, dir, p->gun, p->gun->bullet_sprite);

  // Se criou a nova cabeça, coloca ela na arma
  if (shot_head) {
    p->gun->shots = shot_head;
  }

  return NULL;

}


void destroy_player (struct player *p) {

  if (!p) {
    destroy_joystick (p->joystick);
    pistol_destroy (p->gun);

    al_destroy_bitmap (p->sprite);
    al_destroy_bitmap (p->gun->bullet_sprite);

    free (p);
    p = NULL;
  }
}

int wich_sprite (struct player *p) {

	if (!p || !p->sprite) {
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

int direction (struct player *p) {
	if (!p || !p->joystick) {
		fprintf(stderr, "Erro: ponteiro nulo no direction\n");
		return -1;
	}

	if (p->joystick->right) {
		return 0; // Direita
	} else if (p->joystick->left) {
		return 1; // Esquerda
	}
	return 0; // Parado
}