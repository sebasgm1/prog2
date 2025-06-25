#include <stdlib.h>
#include "Pistol.h"




struct pistol* pistol_create(ALLEGRO_BITMAP *bullet){																				//Implementação da função "pistol_create"

	struct pistol *new_pistol = malloc(sizeof(struct pistol));												//Aloca a memória na heap para uma instância de pistola
	if (!new_pistol) return NULL;
	
    new_pistol->timer = 0;																			//Inicializa o relógio de disparos em zero (pronto para atirar)
	new_pistol->shots = NULL;																			//Inicializa a lista de disparos com NULL; ou seja, sem disparos
	
	new_pistol->bullet_sprite = bullet;
	return new_pistol;																					//Retorna a nova instância de pistola
}

struct bullet* pistol_shot(long x, long y, unsigned char trajectory, struct pistol *gun, ALLEGRO_BITMAP *bullet_sprite){			//Implementação da função "pistol_shot"
	
	struct bullet *new_bullet = (struct bullet*) bullet_create(x, y, trajectory, gun->shots, bullet_sprite);									//Cria uma nova instância de projétil a ser disparado
	if (!new_bullet) return NULL;																		//Verifica o sucesso da alocação de memória; retorna NULL em caso de falha
	return new_bullet;																					//Retorna uma nova instância de projétil
}

void pistol_destroy(struct pistol *element) {
    if (!element) return;

    // Destrói todas as balas
    struct bullet *curr = element->shots;
    while (curr) {
        struct bullet *next = curr->next;
        bullet_destroy(curr);
        curr = next;
    }

    free(element);
    element = NULL;  // Evita dangling pointer
}