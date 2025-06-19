#ifndef __JOYSTICK__ 																												//Guardas de inclusão 
#define __JOYSTICK__																												//Guardas de inclusão 

#include <stdio.h>
#include <allegro5/allegro5.h>														//Biblioteca base do Allegro
#include <allegro5/allegro_primitives.h>

struct joystick {																													//Definição da estrutura de um controle 
	unsigned char right;																											//Botão de movimentação à direta 
	unsigned char left;																												//Botão de movimentação à esquerda 
	unsigned char up;																												  //Botão de movimentação para cima 
	unsigned char down;																												//Botão de movimentação para baixo 
	unsigned char fire; 																											//Botão de disparo
	unsigned char run;																												// Botão que faz ele correr
};																															//Definição do nome da estrutura 

struct joystick *create_joystick (); 

void destroy_joystick (struct joystick *j);



// joystick* joystick_create();																										//Protótipo da função de criação de um controle 
// void joystick_destroy(joystick *element);																							//Protótipo da função de destruição de um controle 
// void joystick_right(joystick *element);																								//Protótipo da função de ativação/desativação do botão de movimentação à direita 
// void joystick_left(joystick *element);																								//Protótipo da função de ativação/desativação do botão de movimentação à esquerda 
// void joystick_up(joystick *element);																								//Protótipo da função de ativação/desativação do botão de movimentação para cima 
// void joystick_down(joystick *element);																								//Protótipo da função de ativação/desativação do botão de movimentação para baixo 
// void joystick_fire(joystick *element);																								//Protótipo da função de ativação/desativação do botão de disparo

#endif																																//Guardas de inclusão 