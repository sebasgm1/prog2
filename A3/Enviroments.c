#include "Enviroments.h"

int Inicio (ALLEGRO_DISPLAY* disp) {

	ALLEGRO_FONT* font = al_load_ttf_font("/home/ibm/sgm24/prog2/A3/fontes/Pixelmax/Pixelmax-Regular.otf", 40, 0);									//Carrega uma fonte padrão para escrever na tela (é bitmap, mas também suporta adicionar fontes ttf
	ALLEGRO_BITMAP *inicio = al_load_bitmap("/home/ibm/sgm24/prog2/A3/sprites/Inicio.png");
	ALLEGRO_TIMER *timer = al_create_timer (1.0 / 60.0);
	ALLEGRO_EVENT_QUEUE *queue = al_create_event_queue ();
	
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
			if (event.keyboard.keycode == ALLEGRO_KEY_ENTER) // Se a tecla pressionada foi ENTER
				sair = 1;											//42: Evento de clique no "X" de fechamento da tela. Encerra o programa graciosamente.
		
			if (event.keyboard.keycode == ALLEGRO_KEY_L) {		// Pra dar load
				sair = 2;
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

void Jogo (ALLEGRO_DISPLAY* disp) {
	ALLEGRO_TIMER *timer = al_create_timer (1.0 / 60.0);
	ALLEGRO_EVENT_QUEUE *queue = al_create_event_queue ();
	ALLEGRO_BITMAP *cenario = al_load_bitmap ("/home/ibm/sgm24/prog2/A3/sprites/fase1.png");

	// Registra três fontes de eventos para a fila
	al_register_event_source (queue ,al_get_display_event_source (disp)); // eventos da tela (fechar a janela)
	al_register_event_source (queue, al_get_keyboard_event_source());			// eventos do teclado (ENTER, ESC, etc),
  al_register_event_source (queue, al_get_mouse_event_source());        // eventos de mouse (click dos botões e rodinha do mouse)
	al_register_event_source (queue, al_get_timer_event_source (timer));	// eventos do timer (usado para animar o texto piscante).

	// Inicia o timer pra tudo começar de vez
	al_start_timer (timer);
	ALLEGRO_EVENT event;		// Estrutura onde os eventos vão ser armazenados à medida que são capturados da queue.

	int sair = 0;						// Flag que fala quando eu vou sair ou não

	while (!sair) {
		// Espera até que algum evento aconteça (tecla, timer ou fechar janela).
		// O programa fica em pausa aqui até o evento chegar. Dai quando chega, insere da variável event
		al_wait_for_event (queue, &event);

		if (event.type == ALLEGRO_EVENT_TIMER) {
			al_draw_bitmap (cenario, 0, 0, 0);
			al_flip_display ();
		}

		if (event.type == ALLEGRO_EVENT_KEY_DOWN || event.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {							// S eu pressionei algo
			if (event.keyboard.keycode == ALLEGRO_KEY_ESCAPE)	{		// Se eu apertar ESC, saio da tela de inicio
				sair = 1;	
			}

      

			// ... outras possíveis teclas pressionadas

		}
		else if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE)	// Quanto o usuário cliaca no X da tela
			exit (0);

	}

	// Destrói tudo que criou"
	al_destroy_timer (timer);
	al_destroy_event_queue (queue);


}

