#ifndef __ENVIROMENTS__
#define __ENVIROMENTS__

#include <stdio.h>
#include <allegro5/allegro5.h>														//Biblioteca base do Allegro 
#include <allegro5/allegro_font.h>													//Biblioteca de fontes do Allegro
#include <allegro5/allegro_ttf.h>										// Fonte TTF
#include <allegro5/allegro_image.h>  								// Para usar al_load_bitmap e al_init_image_addon


void Allegro_init ();

int Inicio (ALLEGRO_DISPLAY* disp);

int Jogo (ALLEGRO_DISPLAY* disp);

// void Fim (ALLEGRO_DISPLAY* disp);


#endif