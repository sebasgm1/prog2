/*

  Neste arquivo estão implementadas a funções descritas no lista.h.
  Cada função tem sua descrição de funcionamente e comentários
  nas linhas que é interessante comentar, que não foram consideradas triviais.

*/

#include <stdio.h>
#include <stdlib.h>
#include "lista.h"

// Cria uma lista vazia.
// Retorno: ponteiro p/ a lista ou NULL em erro.
struct lista_t *lista_cria () {
  struct lista_t *lista = malloc (sizeof(struct lista_t));

  if (lista == NULL)
    return NULL;
  
  lista->prim = NULL;
  lista->ult = NULL;
  lista->tamanho = 0;

  return lista;
}


// Remove todos os itens da lista e libera a memória.
// Retorno: NULL.
struct lista_t *lista_destroi (struct lista_t *lst) {
  if (lst == NULL)
    return NULL;

  struct item_t *aux;
  while (lst->prim != NULL) {
    aux = lst->prim;
    lst->prim = lst->prim->prox;
    free (aux);
  }
  free (lst);
  lst = NULL;

  return lst;
}


// Insere o item na lista na posição indicada;
// se a posição for além do fim da lista ou for -1, insere no fim.
// Retorno: número de itens na lista após a operação ou -1 em erro.
int lista_insere (struct lista_t *lst, struct membro *m, int pos) {
  if (lst == NULL)
    return -1;
  
  struct item_t *novo = malloc (sizeof(struct item_t));
  if (novo == NULL)
    return -1;

  novo->membro = m;
  novo->ant = NULL;
  novo->prox = NULL;
  
  if (pos>lst->tamanho-1 || pos<0) {
    if (lst->tamanho == 0) {
      lst->prim = novo;
      lst->ult = novo;

      lst->tamanho ++;          // tive que fazer o incremento aqui dentro pois o lst->tamanho é usado no IF
      return lst->tamanho;
    }
    novo->ant = lst->ult;
    lst->ult->prox = novo;  
    lst->ult = novo;

    lst->tamanho ++;
    return lst->tamanho;
  }

  struct item_t *aux = lst->prim;
  for (int i=0; i<pos; i++) {
    aux = aux->prox;
  }
  novo->ant = aux->ant;
  novo->prox = aux;

  if (aux->ant != NULL)
    aux->ant->prox = novo;    // o anterior ao aux vai armazenar em seu ponteiro prox o endereço do novo
  else
    lst->prim = novo;

  aux->ant = novo;            // o aux aponta pro novo, que tomou sua posição
  lst->tamanho++;
  return lst->tamanho;
}


// Retira o item da lista da posição indicada.
// se a posição for além do fim da lista ou for -1, retira do fim.
// Retorno: número de itens na lista após a operação ou -1 em erro.
int lista_retira (struct lista_t *lst, struct membro *item, int pos) {
  if (lst == NULL || lst->tamanho<1)
    return -1;
  
  struct item_t *aux;

  // caso em que pos passa o tamanho da lista ou pos<0
  if ((pos>lst->tamanho-1 || pos<0) && lst->tamanho>1) {
    item = lst->ult->membro;
    lst->ult->ant->prox = NULL;   // vai no antepenúltimo item e dá NULL no ponteiro prox dele

    aux = lst->ult->ant;

    free (lst->ult);
    lst->ult = aux;

    lst->tamanho --;
    return lst->tamanho;
  }

  // caso em que a lista tem apenas um item
  if ((lst->tamanho == 1) && (pos <= 0 || pos>lst->tamanho-1)) {
    item = lst->prim->membro;

    free (lst->prim);
    lst->prim = NULL;
    lst->ult = NULL;

    lst->tamanho --;
    return lst->tamanho;
  }

   // caso em que posi é 0 e (lst->elementos) > 1
  if (pos == 0){
    item = lst->prim->membro;

    aux = lst->prim->prox;
    free (lst->prim);
    lst->prim = aux;
    lst->prim->ant = NULL;

    lst->tamanho --;
    return lst->tamanho;
  }

  // caso em que eu quero retirar o último elemento e (lst->elementos) > 1
  if (pos == lst->tamanho-1) {
    item = lst->ult->membro;

    aux = lst->ult->ant;
    free (lst->ult);
    lst->ult = aux;
    lst->ult->prox = NULL;

    lst->tamanho --;
    return lst->tamanho;
  }

  // daqui pra frente são os casos em que 0 < pos < (lst->tamanho)-1
  aux = lst->prim;
  for (int i=0; i<pos; i++) {
    aux = aux->prox;
  }
  item = aux->membro;

  // aqui eu estou fazendo com que o anterior e posterior ao aux se apontem
  aux->prox->ant = aux->ant;
  aux->ant->prox = aux->prox;
  free (aux);

  lst->tamanho --;
  return lst->tamanho;
}


// Informa o membro do item na posição indicada, sem retirá-lo.
// se a posição for além do fim da lista ou for -1, consulta do fim.
// Retorno: número de itens na lista ou -1 em erro.
int lista_consulta (struct lista_t *lst, struct membro *m, int pos) {
  // if (lst == NULL || lst->tamanho<1)
  if (lst == NULL || lst->tamanho<1 || pos>lst->tamanho-1)
    return -1;
  
  // if (pos<0 || pos>lst->tamanho-1) {
  if (pos<0) {
    m = lst->ult->membro;
    return lst->tamanho;
  }

  struct item_t *aux = lst->prim;
  for (int i=0; i<pos; i++) {
    aux = aux->prox;
  }

  m = aux->membro;
  return lst->tamanho;
}


// Informa a posição da 1ª ocorrência do valor indicado na lista.
// Retorno: posição do valor ou -1 se não encontrar ou erro.
int lista_procura (struct lista_t *lst, struct membro *m) {
  if (lst == NULL || lst->tamanho<1)
    return -1;
  
  struct item_t *aux = lst->prim;
  int i = 0;  

  // enquanto nao achar o valor e I nao for maior que o tamanho da fila, vai iterando
  while (aux != NULL && i<=lst->tamanho-1) {
    if (aux->membro == m) {
      return i;
    }
    aux = aux->prox;
    i++;              // ficar de olho nessa linha, n sei se é ++i ou i++
  }
  return -1;
}


// Informa o tamanho da lista (o número de itens presentes nela).
// Retorno: número de itens na lista ou -1 em erro.
int lista_tamanho (struct lista_t *lst) {
  if (lst == NULL || lst->tamanho<0)
    return -1;
  return lst->tamanho;
}


// Imprime o conteúdo da lista do inicio ao fim no formato "item item ...",
// com um espaço entre itens, sem espaços antes/depois, sem newline.
void lista_imprime (struct lista_t *lst) {
  if (lst == NULL || lst->tamanho<1) {
    return;
  }  
  printf ("%d", lst->prim->membro->UID);

  struct item_t *aux = lst->prim;
  for (int i=1; i<lst->tamanho; i++) {      // começa do 1 por que a primeira posição já foi printada
    aux = aux->prox;
    printf (" %d", aux->membro->UID);
  }
  return;
}