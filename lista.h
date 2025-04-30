// Vai ser usada a implementação de lista para a organização dos membros do archive 
// trabalho A1 de Programação 2

// TAD lista de ponteiros para membros do Archive
// Sebastián Galvis Moreno


#ifndef LISTA
#define LISTA

#include "vinac.h"

// estrutura de um item da lista
struct item_t
{
  struct membro *membro ;   // membro a ser apontado
  struct item_t *ant ;		// item anterior
  struct item_t *prox ;	    // próximo item
} ;

// estrutura de uma lista
struct lista_t
{
  int tamanho ;		          // número de itens da lista
  struct item_t *prim ;	    // primeiro item
  struct item_t *ult ;		  // último item
} ;

// Cria uma lista vazia.
// Retorno: ponteiro p/ a lista ou NULL em erro.
struct lista_t *lista_cria ();

// Remove todos os itens da lista e libera a memória.
// Retorno: NULL.
struct lista_t *lista_destroi (struct lista_t *lst);

// Nas operações insere/retira/consulta/procura, a lista inicia na
// posição 0 (primeiro item) e termina na posição TAM-1 (último item).

// Insere o item na lista na posição indicada;
// se a posição for além do fim da lista ou for -1, insere no fim.
// Retorno: número de itens na lista após a operação ou -1 em erro.
int lista_insere (struct lista_t *lst, struct membro *m, int pos);

// Retira o item da lista da posição indicada.
// se a posição for além do fim da lista ou for -1, retira do fim.
// Retorno: número de itens na lista após a operação ou -1 em erro.
int lista_retira (struct lista_t *lst, struct membro *m, int pos);

// Informa o membro do item na posição indicada, sem retirá-lo.
// se a posição for além do fim da lista ou for -1, consulta do fim.
// Retorno: número de itens na lista ou -1 em erro.
int lista_consulta (struct lista_t *lst, struct membro *m, int pos);

// Informa a posição da 1ª ocorrência do membro indicado na lista.
// Retorno: posição do membro ou -1 se não encontrar ou erro.
int lista_procura (struct lista_t *lst, struct membro *m);

// Informa o tamanho da lista (o número de itens presentes nela).
// Retorno: número de itens na lista ou -1 em erro.
int lista_tamanho (struct lista_t *lst);

// Imprime o conteúdo da lista do inicio ao fim no formato "item item ...",
// com um espaço entre itens, sem espaços antes/depois, sem newline.
void lista_imprime (struct lista_t *lst);

#endif