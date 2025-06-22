#include "Enviroments.h"
#include "Player.h"




int Inicio (ALLEGRO_DISPLAY* disp) {

	ALLEGRO_FONT *font = al_load_ttf_font("/Users/sebas/Documents/Prog2/A3/fontes/Pixelmax/Pixelmax-Regular.otf", 40, 0);									//Carrega uma fonte padrão para escrever na tela (é bitmap, mas também suporta adicionar fontes ttf
	ALLEGRO_BITMAP *inicio = al_load_bitmap("/Users/sebas/Documents/Prog2/A3/sprites/Inicio.png");
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
				al_draw_text(font, al_map_rgb(255, 255, 255), 480, 450, ALLEGRO_ALIGN_CENTER, "PRESSIONE ENTER");
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


void draw_hitbox (struct player *p, int down) {

    if (!p) {
        perror ("Erro ao desenhar a hitbox do player\n");
        return;
    }
	
	float x_Lsuperior = p->x; 
	float y_Lsuperior = p->y; 
	float x_Rinferior = p->x + p->x_size; 
	float y_Rinferior = p->y + p->y_size;

	if (down == 5) {
		y_Lsuperior += 60; 
	}

    al_draw_rectangle (x_Lsuperior, y_Lsuperior, x_Rinferior, y_Rinferior, al_map_rgb(255, 255, 0), 2.0); // Desenha a hitbox do player);
}

void rolling (struct player *p, float *x) {
	if (!p) return;

	if (p->x == RIGHT_LIMIT && *x > -1920)
		*x -= p->vel;
	else if (p->x == LEFT_LIMIT && *x < 0)
		*x += p->vel;
	
}


int Jogo (ALLEGRO_DISPLAY* disp) {
	ALLEGRO_TIMER *timer = al_create_timer (1.0 / 60.0);
	ALLEGRO_EVENT_QUEUE *queue = al_create_event_queue ();
	ALLEGRO_BITMAP *cenario = al_load_bitmap ("/Users/sebas/Documents/Prog2/A3/sprites/Fase1Rolling.png");

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

	struct player *p = player_create (105, 220, 10); // Cria o player, que vai ser usado no jogo
	if (!p) return 0;
	
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

	int cooldown = 19;	// Cooldown do tiro ou sprite

	while (!sair) {
		// Espera até que algum evento aconteça (tecla, timer ou fechar janela).
		// O programa fica em pausa aqui até o evento chegar. Dai quando chega, insere da variável event
		al_wait_for_event (queue, &event);

		switch (event.type) {
			case ALLEGRO_EVENT_TIMER:
				al_clear_to_color (al_map_rgb (255, 255, 255)); // Limpa a tela pra receber o próximo frame

				rolling (p, &x_background);
				al_draw_bitmap (cenario, x_background, 0, 0);


				can_move (p);	// Verifica se o player PODE se mover e move ele, ou não

				// Se tiver sprite, desenha
				if (p->sprite) {
					num_sprite = wich_sprite (p);
					draw_hitbox (p, num_sprite);	// Desenha a hitbox do player

					
					// Lógica do pulo com gravidade!
					if (p->joystick->up ) jump_f = 1;
					if (j_vel < -Y_VELOCITY) jump_f = 0;	// Se a velocidade do pulo for menor que -15, desativa o pulo
					
					if (jump_f) {	// Se o joystick estiver pressionado pra cima e o player estiver no ar
						p->y -= j_vel;
						j_vel -= GRAVITY;
						jump_f = 1;			// Flag de pulo ativa
					} else {
						p->y = FLOOR_LIMIT;	// Se ele estiver no chão, não deixa ele subir mais
						j_vel = Y_VELOCITY;	// Reseta a velocidade dele
						jump_f = 0;			// Flag de pulo desativa
					}
					
					// Define qual sprite do player vou pegar
					coluna =  ((num_sprite * al_get_bitmap_width(p->sprite) / 7));		// Calcula qual sprite pegar
					linha = (direction (p) * (al_get_bitmap_height(p->sprite) / 2));	// De qual linha (virado pra direita ou esquerda)

					// Desenha cada sprite dependendo do estado do player
					al_draw_scaled_bitmap (
						p->sprite, 
						coluna, linha, 
						440, 525,
						p->x, p->y, 
						220, 262.5, 
						0
					);

					if (p->gun->shots) {
						for (struct bullet *i = p->gun->shots; i != NULL; i = i->next)
							al_draw_bitmap (i->sprite, i->x, i->y, 0);
						if (p->gun->timer) p->gun->timer--;
					}
				}
				al_flip_display ();
				break;
			
			case ALLEGRO_EVENT_KEY_DOWN:
			case ALLEGRO_EVENT_KEY_UP:
				if (event.keyboard.keycode == ALLEGRO_KEY_ESCAPE)	{		// Se eu apertar ESC, volto pro inicio
					sair = 1;	
				}
				
				// Vai mudar o estado do joystick, mas não necessariamente vai ser aprovado
				move_player (p->joystick, event);


				break;
				
			case ALLEGRO_EVENT_MOUSE_BUTTON_DOWN:
			case ALLEGRO_EVENT_MOUSE_BUTTON_UP:
				
				
				move_player (p->joystick, event);
				break;
			
			case ALLEGRO_EVENT_DISPLAY_CLOSE:
				exit (0);
				break;
		}
	}	

	// Destrói tudo que criou
	al_destroy_timer (timer);
	al_destroy_event_queue (queue);
	destroy_player (p); // Destrói o player que foi criado
	al_destroy_bitmap (cenario);

	return sair;
}	

