// Compilação: gcc Biogun.c Enviroments.c Player.c Joystick.c -o BG $(pkg-config allegro-5 allegro_main-5 allegro_font-5 allegro_image-5 allegro_ttf-5 --libs --cflags)

#include <stdio.h>
#include <allegro5/allegro5.h>										// Biblioteca base do Allegro
#include <allegro5/allegro_font.h>									// Biblioteca de fontes do Allegro
#include <allegro5/allegro_ttf.h>									// Fonte TTF
#include <allegro5/allegro_image.h>  								// Para usar al_load_bitmap e al_init_image_addon
#include <allegro5/allegro_native_dialog.h> 						// Para exibir mensagens de erro, se necessário
#include <allegro5/allegro_primitives.h>


#include "Player.h"
#include "Enviroments.h"


int main(){
	al_init();																		//Faz a preparação de requisitos da biblioteca Allegro
	al_init_image_addon();													// Necessário para carregar imagens
	al_init_font_addon ();
	al_init_ttf_addon();														// Inicia da fonte Addon
	al_install_keyboard();							    				//Habilita a entrada via teclado (eventos de teclado), no programa
	al_install_mouse ();

	ALLEGRO_DISPLAY* disp = al_create_display(960, 540);							//Cria uma janela para o programa, define a largura (x) e a altura (y) da tela em píxeis (320x320, neste caso)
  if (!disp) {
	fprintf(stderr, "Erro ao carregar display\n");
	return -1;
  }

	// Inicia a tela de início do jogo
	int select = 1;
	while (select != 0) {														//Enquanto o valor de select for diferente de 0, o programa continuará rodando
		if (select == 1) { // Tela de início do jogo
			select = Inicio (disp);
		}
		if (select == 2) { // Tela do jogo
			select = Jogo (disp);
		}
		if (select == 3) {  // Tela da fase final (BOSS)
			select = Final (disp);
		}	
		if (select == 4) { // Tela de game over (MORREU)
			select = GameOver (disp);
		}
		if (select == 5) { // Tela de vitória!!!!!!

		}
	}
	


	al_destroy_display(disp);														//Destrutor da tela

	return 0;
}
/*
Wiki: https://github.com/liballeg/allegro_wiki/wiki
    Event: https://www.allegro.cc/manual/5/events.html

Fonts: https://www.dafont.com/pt/search.php?q=pixel
*/