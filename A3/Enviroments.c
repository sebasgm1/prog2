#include "Enviroments.h"
#include "Player.h"




// O limite do chão minimo
#define FLOOR_LIMIT 220


int Inicio (ALLEGRO_DISPLAY* disp) {

	ALLEGRO_FONT *font = al_load_ttf_font("/home/dsbd/prog2/A3/fontes/Pixelmax/Pixelmax-Regular.otf", 40, 0);									//Carrega uma fonte padrão para escrever na tela (é bitmap, mas também suporta adicionar fontes ttf
	ALLEGRO_BITMAP *inicio = al_load_bitmap("/home/dsbd/prog2/A3/sprites/Inicio.png");
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

int witch_sprite (struct player *p) {
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
	} else if (p->joystick->up) {
		num_sprite = 4;	// Jump
	} else if (p->joystick->down) {
		num_sprite = 5;	// Downgun
	} else if (p->joystick->fire) {
		num_sprite = 3;	// Rungun
	} else if (p->joystick->left) {
		num_sprite = 1;	// Run1
	} else if (p->joystick->right) {
		num_sprite = 1;	// Run2
	} else if (p->joystick->up && p->joystick->fire) {
		num_sprite = 4;	// Jumpgun
	} else if (p->joystick->down && p->joystick->fire) {
		num_sprite = 5;	// Downgun
	} else {
		num_sprite = 0;	// Stand
	}

	return num_sprite;

} 




int Jogo (ALLEGRO_DISPLAY* disp) {
	ALLEGRO_TIMER *timer = al_create_timer (1.0 / 60.0);
	ALLEGRO_EVENT_QUEUE *queue = al_create_event_queue ();
	ALLEGRO_BITMAP *cenario = al_load_bitmap ("/home/dsbd/prog2/A3/sprites/fase1.png");

	enum Direction {right , left};
	enum Sprite {stand, run1, run2, rungun, jump, downgun, jumpgun};

	if (timer == NULL || queue == NULL || cenario == NULL) {
		fprintf(stderr, "Erro ao carregar a fonte ou bitmap\n");
		return -1;
	}

	struct player *p = player_create (); // Cria o player, que vai ser usado no jogo
	if (!p) return 0;
	
	al_register_event_source (queue ,al_get_display_event_source (disp)); // eventos da tela (fechar a janela)
	al_register_event_source (queue, al_get_keyboard_event_source());			// eventos do teclado (ENTER, ESC, etc),
  	al_register_event_source (queue, al_get_mouse_event_source());       	 // eventos de mouse (click dos botões e rodinha do mouse)
	al_register_event_source (queue, al_get_timer_event_source (timer));	// eventos do timer (usado para animar o texto piscante).

	// Inicia o timer pra tudo começar de vez
	al_start_timer (timer);
	ALLEGRO_EVENT event;		// Estrutura onde os eventos vão ser armazenados à medida que são capturados da queue.

	int sair = 0;						// Flag que fala quando eu vou sair ou não
	float linha = 0, coluna = 0;
	int dir = 0;	// Direction
	int num_sprite = 0;	// Sprite que vai ser desenhado


	while (!sair) {
		// Espera até que algum evento aconteça (tecla, timer ou fechar janela).
		// O programa fica em pausa aqui até o evento chegar. Dai quando chega, insere da variável event
		al_wait_for_event (queue, &event);

		switch (event.type) {
			case ALLEGRO_EVENT_TIMER:
				al_clear_to_color (al_map_rgb (0, 0, 0)); // Limpa a tela pra receber o próximo frame

				al_draw_bitmap (cenario, 0, 0, 0);
				can_move (p, FLOOR_LIMIT);	// Verifica se o player PODE se mover e move ele, ou não
				if (p->sprite) {
					if (p->joystick->left) {		// Se o joystick estiver pressionado pra direita
						dir = left;		// Direção é direita
					}
					else if (p->joystick->right) {	// Se o joystick estiver pressionado pra esquerda
						dir = right;			// Direção é esquerda
					}

					num_sprite = witch_sprite (p);

					if (num_sprite == 2)
						num_sprite = 1;		// Reseta o número do sprite,4
					else if (num_sprite == 1)
						num_sprite = 2;		// Reseta o número do sprite,	
					
					coluna =  ((num_sprite * al_get_bitmap_width(p->sprite) / 5));	// Divide a largura da imagem pelo número de colunas
					linha = (dir * (al_get_bitmap_height(p->sprite) / 2));	// Divide a altura da imagem pelo número de linhas

					// al_draw_bitmap (p->sprite, p->x, p->y, 0);
					al_draw_scaled_bitmap (
						p->sprite, 
						coluna, linha, 
						440, 525,
						p->x, p->y, 
						220, 262.5, 
						0);
				
				
				
				
				
				}
				al_flip_display ();
				break;
			
			case ALLEGRO_EVENT_KEY_DOWN:
			case ALLEGRO_EVENT_KEY_UP:
				if (event.keyboard.keycode == ALLEGRO_KEY_ESCAPE)	{		// Se eu apertar ESC, volto pro inicio
					sair = 1;	
				}
				
				// Vai mudar o estado do joystick, mas não necessriamente vai ser aprovado
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
#
