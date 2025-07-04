#include "Enviroments.h"
#include "Player.h"

typedef struct {
	int activated;
	float trigger_x;
	struct player* enemy;
} SpawnTrigger;



int Inicio (ALLEGRO_DISPLAY* disp) {

	ALLEGRO_FONT *font = al_load_ttf_font("/home/sebasitan/Documents/prog2/A3/fontes/Pixelmax/Pixelmax-Regular.otf", 40, 0);									//Carrega uma fonte padrão para escrever na tela (é bitmap, mas também suporta adicionar fontes ttf
	ALLEGRO_BITMAP *inicio = al_load_bitmap("/home/sebasitan/Documents/prog2/A3/sprites/Inicio.png");
	ALLEGRO_TIMER *timer = al_create_timer (1.0 / 60.0);
	ALLEGRO_EVENT_QUEUE *queue = al_create_event_queue ();

	if (font == NULL || inicio == NULL || timer == NULL || queue == NULL) {
		fprintf(stderr, "Erro ao carregar a fonte ou bitmap\n");
		return 0;
	}
	
	// Registra três fontes de eventos para a fila
	al_register_event_source (queue ,al_get_display_event_source (disp)); // eventos da tela (fechar a janela)
	al_register_event_source (queue, al_get_keyboard_event_source());			// eventos do teclado (ENTER, ESC, etc),
	al_register_event_source (queue, al_get_timer_event_source (timer));	// eventos do timer (usado para animar o texto piscante).

	// Inicia o timer pra tudo começar de vez
	al_start_timer (timer);
	
	// Variáveis de controle
	int sair = 0;
	char mostrar_texto = 1;
	double t_texto = 0;		
	ALLEGRO_EVENT event;		// Estrutura onde os eventos vão ser armazenados à medida que são capturados da queue.

	while (!sair) {
		// Espera até que algum evento aconteça (tecla, timer ou fechar janela).
		// O programa fica em pausa aqui até o evento chegar. Dai quando chega, insere da variável event
		al_wait_for_event(queue, &event);
		
		// ALLEGRO_EVENT_TIMER é 30
		if (event.type == ALLEGRO_EVENT_TIMER){			//O evento tipo 30 indica um evento de relógio, ou seja, verificação se a tela deve ser atualizada (conceito de FPS)
			
			t_texto += 1.0 / 60.0;
			if (t_texto >= 0.5) {			// A cada 0.5s ele inverte, mostra e some, mostra e some
				mostrar_texto = !mostrar_texto;
				t_texto = 0;
			}

			// al_clear_to_color(al_map_rgb(0, 0, 0));									//Substitui tudo que estava desenhado na tela por um fundo preto
			al_draw_bitmap (inicio,0 ,0 , 0);							// Coloca na tela o fundo do BIOGUN de incio

			if (mostrar_texto) {
				al_draw_text(font, al_map_rgb(255, 255, 255), 480, 450, ALLEGRO_ALIGN_CENTER, "PRESS ENTER");
			}
			al_flip_display();	

		}
		// ALLEGRO_EVENT_DISPLAY_CLOSE == 42, quando o usuário clica no X no canto da tela
		else if (event.type == ALLEGRO_EVENT_KEY_DOWN) {	// Se um evento de tecla pressionada foi chamado

			if (event.keyboard.keycode == ALLEGRO_KEY_ESCAPE)		// Se eu apertar ESC, saio da tela de inicio
				exit (0);	
			
			if (event.keyboard.keycode == ALLEGRO_KEY_ENTER) // Se a tecla pressionada foi ENTER
				sair = 2;											//42: Evento de clique no "X" de fechamento da tela. Encerra o programa graciosamente.
		
			if (event.keyboard.keycode == ALLEGRO_KEY_L) {		// Pra dar load
				sair = 3;
			}

			// ... Podem ter mais casos
		
		}
		else if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) { // ALLEGRO_EVENT_DISPLAY_CLOSE == 42, quando o usuário clica no X no canto da tela
			exit (0);
		}
	}

	// Destrói tudo que criou

	al_destroy_font(font);															//Destrutor da fonte padrão
	al_destroy_bitmap (inicio);													// Destrutor de sprites
	al_destroy_timer (timer);
	al_destroy_event_queue (queue);
	

	// 1: Enter
	// 2: L (load)
	return sair;
	
}




void rolling (struct player *p, float *x, int right_limit) {
	if (!p || !p->joystick) return;

	int vel = p->vel;
	if (p->joystick->run)
		vel = vel * 2;

	if (p->joystick->right && p->x == RIGHT_LIMIT && *x > right_limit)
		*x -= vel;
	else if (p->joystick->left && p->x == LEFT_LIMIT && *x < 0)
		*x += vel;
	
}

int check_player_death(struct player *p, struct player **enemies, int enemy_count, SpawnTrigger *triggers, int num_triggers) {
    if (!p) return 0;
    
    for (int i = 0; i < enemy_count; i++) {
        if (!enemies[i] || !enemies[i]->active) continue;
        
        int result = check_kill(p, enemies[i]);
        
        if (result == 1) {  // Player morreu
            destroy_player(&p);
            return 1;
        }
        else if (result == 2) {  // Inimigo morreu
            // Atualiza triggers
			for (int j = 0; j < num_triggers; j++) {
				if (triggers[j].enemy == enemies[i]) {
					triggers[j].enemy = NULL;
					triggers[j].activated = 0;
				}
			}
			destroy_player(&enemies[i]);
      	}
    }
    return 0;
}


int Jogo (ALLEGRO_DISPLAY* disp) {
	ALLEGRO_FONT *font = al_load_ttf_font("/home/sebasitan/Documents/prog2/A3/fontes/Pixelmax/Pixelmax-Regular.otf", 40, 0);									//Carrega uma fonte padrão para escrever na tela (é bitmap, mas também suporta adicionar fontes ttf
	ALLEGRO_TIMER *timer = al_create_timer (1.0 / 60.0);
	ALLEGRO_EVENT_QUEUE *queue = al_create_event_queue ();
	ALLEGRO_BITMAP *cenario = al_load_bitmap ("/home/sebasitan/Documents/prog2/A3/sprites/Fase1Rolling.png");
	
	// Sprite do jogador e inimigos
	ALLEGRO_BITMAP *player_sprite = al_load_bitmap ("/home/sebasitan/Documents/prog2/A3/sprites/player440x525.png");
	ALLEGRO_BITMAP *enemy_sprite = al_load_bitmap ("/home/sebasitan/Documents/prog2/A3/sprites/virus.png");

	ALLEGRO_BITMAP *player_bullet = al_load_bitmap ("/home/sebasitan/Documents/prog2/A3/sprites/Bullet.png");
	ALLEGRO_BITMAP *enemy_bullet = al_load_bitmap ("/home/sebasitan/Documents/prog2/A3/sprites/virusBullet.png");

	if (!player_sprite || !enemy_sprite || !player_bullet || !enemy_bullet) {
		fprintf(stderr, "Erro ao carregar as sprites\n");
		return -1;
	}

	enum Direction {right , left};
	enum Sprite {stand, run1, run2, rungun, jump, downgun, jumpgun};
	float x_background = 0;

	if (timer == NULL || queue == NULL || cenario == NULL) {
		fprintf(stderr, "Erro ao carregar a fonte ou bitmap\n");
		return -1;
	}

	if (!al_init_primitives_addon()) {
		perror ("Erro de inicialização do addon de primitivas\n");
		return -1;
	}

	

	struct player *p  = player_create (105, 220, 20, player_sprite, player_bullet); // Cria o player, que vai ser usado no jogo
	struct player *e1 = player_create (800, 220, 3, enemy_sprite, enemy_bullet);
	struct player *e2 = player_create (50, 220, 3, enemy_sprite, enemy_bullet);
	struct player *e3 = player_create (800, 220, 3, enemy_sprite, enemy_bullet);
	struct player *e4 = player_create (50, 220, 3, enemy_sprite, enemy_bullet);
	struct player *e5 = player_create (700, 220, 3, enemy_sprite, enemy_bullet);
	struct player *e6 = player_create (800, 220, 3, enemy_sprite, enemy_bullet);

	if (!p || !e1 || !e2 || !e3 || !e4 || !e5 || !e6) return 0;

	
	SpawnTrigger triggers[] = {
		{0, 0, e1},  // Inimigo 1 aparece quando x_background <= -300
		{0, -300, e2},  // Inimigo 2 aparece quando x_background <= -400
		{0, -800, e3},  // Inimigo 3 aparece quando x_background <= -600
		{0, -900, e4},  // Inimigo 4 aparece quando x_background <= -900
		{0, -1500, e5}, // Inimigo 5 aparece quando x_background <= -1500
		{0, -1700, e6}, // Inimigo 6 aparece quando x_background <= -1700
		// Adicione mais conforme necessário
	};

	int num_triggers = sizeof(triggers)/sizeof(triggers[0]);


	
	al_register_event_source (queue ,al_get_display_event_source (disp)); // eventos da tela (fechar a janela)
	al_register_event_source (queue, al_get_keyboard_event_source());			// eventos do teclado (ENTER, ESC, etc),
  	al_register_event_source (queue, al_get_mouse_event_source());       	 // eventos de mouse (click dos botões e rodinha do mouse)
	al_register_event_source (queue, al_get_timer_event_source (timer));	// eventos do timer (usado para animar o texto piscante).

	// Inicia o timer pra tudo começar de vez
	al_start_timer (timer);
	ALLEGRO_EVENT event;		// Estrutura onde os eventos vão ser armazenados à medida que são capturados da queue.


	int sair = 0, dir = 0, jump_f = 0;						// sair: flag pra sair do jogo; dir: direção da sprite; jump_f: flag de pulo
	float linha = 0, coluna = 0, j_vel = Y_VELOCITY;
	int num_sprite = 0;	// Sprite que vai ser desenhado
	int morto = 0, state1 = 0;
	float timer1 = 1;


	while (!sair) {
		// Espera até que algum evento aconteça (tecla, timer ou fechar janela).
		// O programa fica em pausa aqui até o evento chegar. Dai quando chega, insere da variável event
		al_wait_for_event (queue, &event);

		switch (event.type) {
			case ALLEGRO_EVENT_TIMER:
				// if (p == NULL) {  // Se o jogador já foi destruído
				// 	sair = 4;
				// 	break;
				// }
			
				al_clear_to_color (al_map_rgb (255, 255, 255)); // Limpa a tela pra receber o próximo frame

				if (p)
					rolling (p, &x_background, -1920);
				al_draw_bitmap (cenario, x_background, 0, 0);


				// Verifica todos os triggers
				for (int i = 0; i < num_triggers; i++) {
					if (!triggers[i].activated && x_background <= triggers[i].trigger_x) {
						triggers[i].activated = 1;
						printf("Inimigo %d spawnado!\n", i+2); // Debug
					}

				}

				if (x_background <= -1700) {
					al_draw_text(font, al_map_rgb(255, 255, 255), 480, 450, ALLEGRO_ALIGN_CENTER, "FINAL BOSS");
					al_draw_text(font, al_map_rgb(255, 255, 0), 480, 500, ALLEGRO_ALIGN_CENTER, "PRESSIONE F");
				}


				// Processa inimigos spawnados
				for (int i = 0; i < num_triggers; i++) {
					if (triggers[i].activated && triggers[i].enemy && triggers[i].enemy->active) {
						if (!triggers[i].enemy->sprite) {
							fprintf(stderr, "Erro: sprite do inimigo %d não carregada\n", i);
							continue;
						}
						// Só processa se o inimigo estiver ativo
						move_enemy(triggers[i].enemy, p, &state1, &timer1);
						
						if (triggers[i].enemy->gun) {  // Verificação adicional
							bullet_move(&triggers[i].enemy->gun->shots, triggers[i].enemy->gun->bullet_sprite);
						}
						
						num_sprite = witch_virus (triggers[i].enemy);

						coluna =  (num_sprite * (al_get_bitmap_width(p->sprite) / 3));		// Calcula qual sprite pegar
						linha = 89;	// De qual linha (virado pra direita ou esquerda)
					
						// if (triggers[i].enemy && triggers[i].enemy->sprite) {
							al_draw_scaled_bitmap(
								triggers[i].enemy->sprite, 
								triggers[i].enemy->last_direction * (al_get_bitmap_width (triggers[i].enemy->sprite)/3), 
								linha, 
								440, 525,
								triggers[i].enemy->x, triggers[i].enemy->y, 
								220, 262.5, 
								0
							);
						// }
						
						check_collision(p, triggers[i].enemy);
						check_collision(triggers[i].enemy, p);

					}
				}

				struct player *enemies[] = {e1, e2, e3, e4, e5, e6};
				if (check_player_death(p, enemies, 6, triggers, num_triggers)) {
					return 4;
				}

				if (e1 && e1->active && check_kill(p, e1) == 2) {
					destroy_player(&e1);
					// Atualiza triggers
					for (int i = 0; i < num_triggers; i++) {
						if (triggers[i].enemy == e1) {
							triggers[i].enemy = NULL;
						}
					}
				}

				if (e2 && e2->active && check_kill(p, e2) == 2) {
					destroy_player(&e2);
					// Atualiza triggers
					for (int i = 0; i < num_triggers; i++) {
						if (triggers[i].enemy == e2) {
							triggers[i].enemy = NULL;
						}
					}
				}

				if (e3 && e3->active && check_kill(p, e3) == 2) {
					destroy_player(&e3);
					// Atualiza triggers
					for (int i = 0; i < num_triggers; i++) {
						if (triggers[i].enemy == e3) {
							triggers[i].enemy = NULL;
						}
					}
				}

				if (e4 && e4->active && check_kill(p, e4) == 2) {
					destroy_player(&e4);
					// Atualiza triggers
					for (int i = 0; i < num_triggers; i++) {
						if (triggers[i].enemy == e4) {
							triggers[i].enemy = NULL;
						}
					}
				}

				if (e5 && e5->active && check_kill(p, e5) == 2) {
					destroy_player(&e5);
					// Atualiza triggers
					for (int i = 0; i < num_triggers; i++) {
						if (triggers[i].enemy == e5) {
							triggers[i].enemy = NULL;
						}
					}
				}

				if (e6 && e6->active && check_kill(p, e6) == 2) {
					destroy_player(&e6);
					// Atualiza triggers
					for (int i = 0; i < num_triggers; i++) {
						if (triggers[i].enemy == e6) {
							triggers[i].enemy = NULL;
						}
					}
				}
				



				// Vai mover o player e ver se ele atira
				if (p) {
					can_move (p);	// Verifica se o player PODE se mover e move ele, ou não
					bullet_move (&p->gun->shots, p->gun->bullet_sprite);	// Move e checa se os shots sairam do mapa
				

					num_sprite = wich_sprite (p);

					// Lógica do pulo com gravidade!
					if (p->joystick->up ) jump_f = 1;
					if (j_vel < -Y_VELOCITY) jump_f = 0;	// Se a velocidade do pulo for menor que -15, desativa o pulo
					
					if (jump_f) {	// Se o joystick estiver pressionado pra cima e o player estiver no ar
						p->y -= j_vel;
						j_vel -= GRAVITY;
					} else {

						p->y = FLOOR_LIMIT;	// Se ele estiver no chão, não deixa ele subir mais
						j_vel = Y_VELOCITY;	// Reseta a velocidade dele
					}
					
					if (p->joystick->left || p->joystick->right)
						direction (p, &dir);
					else dir = p->last_direction;

					// Define qual sprite do player vou pegar
					coluna =  ((num_sprite * al_get_bitmap_width(p->sprite) / 7));		// Calcula qual sprite pegar
					linha = (dir * (al_get_bitmap_height(p->sprite) / 2));	// De qual linha (virado pra direita ou esquerda)
					
					// Desenha cada sprite dependendo do estado do player
					al_draw_scaled_bitmap (
						p->sprite, 
						coluna, linha, 
						440, 525,
						p->x, p->y, 
						220, 262.5, 
						0
					);

						// Desenha as sprites das balas
					if (p->gun->shots) {
						for (struct bullet *i = p->gun->shots; i != NULL; i = i->next) {
							if (!i->trajectory)
								al_draw_scaled_bitmap(
									i->sprite,
									0, 0,
									al_get_bitmap_width(i->sprite), al_get_bitmap_height(i->sprite),
									i->x, i->y,
									al_get_bitmap_width(i->sprite) * 0.7, al_get_bitmap_height(i->sprite) * 0.7,
									i->trajectory ? ALLEGRO_FLIP_HORIZONTAL : 0
								);
						}
					}
				}




				if (p) // Mostra o display se o jogador estiver vivo
					al_flip_display ();
				break;
			
			case ALLEGRO_EVENT_KEY_DOWN:
			case ALLEGRO_EVENT_KEY_UP:
				if (event.keyboard.keycode == ALLEGRO_KEY_ESCAPE)	{		// Se eu apertar ESC, volto pro inicio
					sair = 1;
					// acho que eu poderia adicionar a pausa o jogo bem aqui ***********
				}

				if (x_background <= -1700 && event.keyboard.keycode == ALLEGRO_KEY_F) {
					sair = 3;
				}
				
				// Vai mudar o estado do joystick, mas não necessariamente vai ser aprovado
				if (p)
					move_player (p->joystick, event);


				break;
				
			case ALLEGRO_EVENT_MOUSE_BUTTON_DOWN:
			case ALLEGRO_EVENT_MOUSE_BUTTON_UP:
				
				if (p)
					move_player (p->joystick, event);
				break;
			
			case ALLEGRO_EVENT_DISPLAY_CLOSE:
				sair = 4;
				break;
		}
		if (sair == 4) {
			break;
		}
		// printf ("SAIR: %d\n", sair);
	}	


	// Destrói tudo que criou
	al_destroy_font(font);
	al_destroy_timer (timer);
	al_destroy_event_queue (queue);
	al_destroy_bitmap (cenario);
	al_destroy_bitmap (player_sprite);
	al_destroy_bitmap (enemy_sprite);
	al_destroy_bitmap (player_bullet);
	al_destroy_bitmap (enemy_bullet);

	return sair;
}	





int Final (ALLEGRO_DISPLAY* disp) {
	ALLEGRO_TIMER *timer = al_create_timer (1.0 / 60.0);
	ALLEGRO_FONT *font = al_load_ttf_font("/home/sebasitan/Documents/prog2/A3/fontes/Pixelmax/Pixelmax-Regular.otf", 40, 0);									//Carrega uma fonte padrão para escrever na tela (é bitmap, mas também suporta adicionar fontes ttf
	ALLEGRO_EVENT_QUEUE *queue = al_create_event_queue ();
	ALLEGRO_BITMAP *cenario = al_load_bitmap ("/home/sebasitan/Documents/prog2/A3/sprites/FINALRolling.png");

	ALLEGRO_BITMAP *player_sprite = al_load_bitmap ("/home/sebasitan/Documents/prog2/A3/sprites/player440x525.png");	
	ALLEGRO_BITMAP *boss_sprite = al_load_bitmap ("/home/sebasitan/Documents/prog2/A3/sprites/Boss.png");

	ALLEGRO_BITMAP *bullet = al_load_bitmap ("/home/sebasitan/Documents/prog2/A3/sprites/Bullet.png");
	ALLEGRO_BITMAP *boss_bullet = al_load_bitmap ("/home/sebasitan/Documents/prog2/A3/sprites/BossBullet.png");


	struct player *p = player_create (105, 220, 20, player_sprite, bullet); // Cria o player, que vai ser usado no jogo
	struct player *boss = player_create (0, 220, 100, boss_sprite, boss_bullet); // Cria o boss, que vai ser usado no jogo


	if (!p || !boss) {
		fprintf(stderr, "Erro ao criar o player ou boss\n");
		return -1;
	}


	if(!timer || !font || !queue || !cenario) {
		perror ("Erro ao carregar elementos da fase final\n");
		al_destroy_font(font);															//Destrutor da fonte padrão
		al_destroy_bitmap (cenario);													// Destrutor de sprites
		al_destroy_timer (timer);
		al_destroy_event_queue (queue);
		return -1;
    }

	SpawnTrigger trigger = {0, -800, boss};  // Boss aparece quando x_background <= -800

	al_register_event_source(queue, al_get_timer_event_source(timer));
    al_register_event_source(queue, al_get_display_event_source(disp));
    al_register_event_source(queue, al_get_keyboard_event_source());

	al_start_timer (timer);
	ALLEGRO_EVENT event;

	printf ("Entrei na fase final!\n");
	int sair = 0, state1 = 0, num_sprite = 0, dir = 0, jump_f = 0;						// sair: flag pra sair do jogo; state1: estado do boss; num_sprite: sprite que vai ser desenhado; dir: direção da sprite; jump_f: flag de pulo
	float timer1 = 1;
	float x_background = 0;	// Posição do background
	float linha = 0, coluna = 0, j_vel = Y_VELOCITY;


	while (!sair) {
		al_wait_for_event (queue, &event);


		switch (event.type) {
			case (ALLEGRO_EVENT_TIMER):
				al_clear_to_color(al_map_rgb(0, 0, 0));									//Substitui tudo que estava desenhado na tela por um fundo preto

				if (p)
					rolling (p, &x_background, -960);
				al_draw_bitmap (cenario, x_background, 0, 0);


				// Testa se o boss pode ser ativado
				if (!trigger.activated && (x_background <= trigger.trigger_x)) {
					trigger.activated = 1;
					printf ("Boss ativado!\n");
				}

				// Se o boss estiver ativado, desenha ele
				if (trigger.activated && trigger.enemy && trigger.enemy->active) {
					if (!trigger.enemy->sprite) {
						fprintf(stderr, "Erro: sprite do inimigo BOSS não carregada\n");
						continue;
					}

					// Só processa se o inimigo estiver ativo
					move_boss(trigger.enemy, p, &state1, &timer1);
					
					if (trigger.enemy->gun) {  // Verificação adicional
						bullet_move(&trigger.enemy->gun->shots, trigger.enemy->gun->bullet_sprite);
					}
					
					num_sprite = witch_virus (trigger.enemy);

					coluna =  (num_sprite * (al_get_bitmap_width(p->sprite) / 3));		// Calcula qual sprite pegar
					linha = 89;	// De qual linha (virado pra direita ou esquerda)
				
					al_draw_scaled_bitmap(
						trigger.enemy->sprite, 
						trigger.enemy->last_direction * (al_get_bitmap_width (trigger.enemy->sprite)/3), 
						linha, 
						440, 525,
						trigger.enemy->x, trigger.enemy->y, 
						220, 262.5, 
						0
					);
					
					check_collision(p, trigger.enemy);
					check_collision(trigger.enemy, p);

				}

				// Vai mover o player e ver se ele atira
				if (p) {
					can_move (p);	// Verifica se o player PODE se mover e move ele, ou não
					bullet_move (&p->gun->shots, p->gun->bullet_sprite);	// Move e checa se os shots sairam do mapa
				

					num_sprite = wich_sprite (p);

					// Lógica do pulo com gravidade!
					if (p->joystick->up ) jump_f = 1;
					if (j_vel < -Y_VELOCITY) jump_f = 0;	// Se a velocidade do pulo for menor que -15, desativa o pulo
					
					if (jump_f) {	// Se o joystick estiver pressionado pra cima e o player estiver no ar
						p->y -= j_vel;
						j_vel -= GRAVITY;
					} else {

						p->y = FLOOR_LIMIT;	// Se ele estiver no chão, não deixa ele subir mais
						j_vel = Y_VELOCITY;	// Reseta a velocidade dele
					}
					
					if (p->joystick->left || p->joystick->right)
						direction (p, &dir);
					else dir = p->last_direction;

					// Define qual sprite do player vou pegar
					coluna =  ((num_sprite * al_get_bitmap_width(p->sprite) / 7));		// Calcula qual sprite pegar
					linha = (dir * (al_get_bitmap_height(p->sprite) / 2));	// De qual linha (virado pra direita ou esquerda)
					
					// Desenha cada sprite dependendo do estado do player
					al_draw_scaled_bitmap (
						p->sprite, 
						coluna, linha, 
						440, 525,
						p->x, p->y, 
						220, 262.5, 
						0
					);

						// Desenha as sprites das balas
					if (p->gun->shots) {
						for (struct bullet *i = p->gun->shots; i != NULL; i = i->next) {
							if (!i->trajectory)
								al_draw_scaled_bitmap(
									i->sprite,
									0, 0,
									al_get_bitmap_width(i->sprite), al_get_bitmap_height(i->sprite),
									i->x, i->y,
									al_get_bitmap_width(i->sprite) * 0.7, al_get_bitmap_height(i->sprite) * 0.7,
									i->trajectory ? ALLEGRO_FLIP_HORIZONTAL : 0
								);
						}
					}
				}

				// Vai checar se alguem morreu
				if (check_player_death(p, &boss, 6, &trigger, 1)) {
					return 4;
				}

				if (boss && boss->active && check_kill(p, boss) == 2) {
					destroy_player(&boss);
					trigger.enemy = NULL;
				}






				al_flip_display ();
			break;

			case (ALLEGRO_EVENT_KEY_DOWN):
			case (ALLEGRO_EVENT_KEY_UP):
				if (event.keyboard.keycode == ALLEGRO_KEY_ESCAPE)	{		// Se eu apertar ESC, volto pro inicio
					sair = 1;
					// acho que eu poderia adicionar a pausa o jogo bem aqui ***********
				}

				if (x_background <= -1700 && event.keyboard.keycode == ALLEGRO_KEY_F) {
					sair = 3;
				}
				
				// Vai mudar o estado do joystick, mas não necessariamente vai ser aprovado
				if (p)
					move_player (p->joystick, event);
	
			break;

			case (ALLEGRO_EVENT_DISPLAY_CLOSE):
				exit(0);
					
			break;



		}


	}



	al_destroy_font(font);															//Destrutor da fonte padrão
	al_destroy_bitmap (cenario);													// Destrutor de sprites
	al_destroy_timer (timer);
	al_destroy_event_queue (queue);

	al_destroy_bitmap (player_sprite);
	al_destroy_bitmap (boss_sprite);
	al_destroy_bitmap (bullet);
	al_destroy_bitmap (boss_bullet);

	return sair;

}

int GameOver (ALLEGRO_DISPLAY* disp) {
	ALLEGRO_TIMER *timer = al_create_timer (1.0 / 60.0);
	ALLEGRO_FONT *font = al_load_ttf_font("/home/sebasitan/Documents/prog2/A3/fontes/Pixelmax/Pixelmax-Regular.otf", 40, 0);									//Carrega uma fonte padrão para escrever na tela (é bitmap, mas também suporta adicionar fontes ttf
	ALLEGRO_EVENT_QUEUE *queue = al_create_event_queue ();
	ALLEGRO_BITMAP *cenario = al_load_bitmap ("/home/sebasitan/Documents/prog2/A3/sprites/GameOver.png");

	if (font == NULL || cenario == NULL || timer == NULL || queue == NULL) {
		fprintf(stderr, "Erro ao carregar a fonte ou bitmap\n");
		return 0;
	}



	// Registra três fontes de eventos para a fila
	al_register_event_source (queue ,al_get_display_event_source (disp)); // eventos da tela (fechar a janela)
	al_register_event_source (queue, al_get_keyboard_event_source());			// eventos do teclado (ENTER, ESC, etc),
	al_register_event_source (queue, al_get_timer_event_source (timer));	// eventos do timer (usado para animar o texto piscante).

	
		// Inicia o timer pra tudo começar de vez
		al_start_timer (timer);
	
		// Variáveis de controle
		int sair = 0;
		char mostrar_texto1 = 1, mostrar_texto2 = 1;
		double t_texto1 = 0, t_texto2 = 0;		
		ALLEGRO_EVENT event;		// Estrutura onde os eventos vão ser armazenados à medida que são capturados da queue.
	
		while (!sair) {
			// Espera até que algum evento aconteça (tecla, timer ou fechar janela).
			// O programa fica em pausa aqui até o evento chegar. Dai quando chega, insere da variável event
			al_wait_for_event(queue, &event);
			
			// ALLEGRO_EVENT_TIMER é 30
			if (event.type == ALLEGRO_EVENT_TIMER){			//O evento tipo 30 indica um evento de relógio, ou seja, verificação se a tela deve ser atualizada (conceito de FPS)
				
				t_texto1 += 1.0 / 60.0;
				if (t_texto1 >= 0.5) {			// A cada 0.5s ele inverte, mostra e some, mostra e some
					mostrar_texto1 = !mostrar_texto1;
					t_texto1 = 0;
				}

				t_texto2 += 1.0 / 60.0;
				if (t_texto2 >= 1.5) {			// A cada 1s ele inverte, mostra e some, mostra e some
					mostrar_texto1 = !mostrar_texto1;
					t_texto2 = 0;
				}
	
				// al_clear_to_color(al_map_rgb(0, 0, 0));									//Substitui tudo que estava desenhado na tela por um fundo preto
				al_draw_bitmap (cenario,0 ,0 , 0);							// Coloca na tela o fundo do BIOGUN de incio
	
				if (mostrar_texto1) {
					al_draw_text(font, al_map_rgb(255, 255, 255), 480, 450, ALLEGRO_ALIGN_CENTER, "PRESS ENTER");
				}

				if (mostrar_texto1) {
					al_draw_text(font, al_map_rgb(255, 0, 0), 480, 350, ALLEGRO_ALIGN_CENTER, "VIRUS DOWNLOADED");
				}
				al_flip_display();	
	
			}
			// ALLEGRO_EVENT_DISPLAY_CLOSE == 42, quando o usuário clica no X no canto da tela
			else if (event.type == ALLEGRO_EVENT_KEY_DOWN) {	// Se um evento de tecla pressionada foi chamado
	
				if (event.keyboard.keycode == ALLEGRO_KEY_ESCAPE)		// Se eu apertar ESC, saio da tela de cenario
					exit (0);	
				
				if (event.keyboard.keycode == ALLEGRO_KEY_ENTER) // Se a tecla pressionada foi ENTER
					sair = 1;											//42: Evento de clique no "X" de fechamento da tela. Encerra o programa graciosamente.
			
				if (event.keyboard.keycode == ALLEGRO_KEY_L) {		// Pra dar load
					sair = 3;
				}
	
				// ... Podem ter mais casos
			
			}
			else if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) { // ALLEGRO_EVENT_DISPLAY_CLOSE == 42, quando o usuário clica no X no canto da tela
				exit (0);
			}
		}
	
		// Destrói tudo que criou
	
		al_destroy_font(font);															//Destrutor da fonte padrão
		al_destroy_bitmap (cenario);													// Destrutor de sprites
		al_destroy_timer (timer);
		al_destroy_event_queue (queue);
		
	
		// 1: Enter
		// 2: L (load)
		return sair;
}