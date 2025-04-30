#ifndef __ARCHIVE__
#define __ARCHIVE__

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>     // Para usar o Stat
//  #include <unistd.h>    // Para usar funções como access(), read(), write(), close(), etc.
#include <sys/types.h>    // Para usar os types que o Stat tem, tipo o off_t
#include <time.h>         // Para usar funções como ctime(), localtime(), strftime(), etc.
#include "lista.h"


struct diretorio {
  struct lista_t lista [1000];  // Lista de membros
  struct lista_t **lista2;      // Ponteiro para a lista que contém todos os memebros

  char *buffer[];               // Ponteiro pra vetor de char, eu mudo dinamicamente o tamanho dele de acordo com o arquivo
};


struct membro {
  char nome [1024];       // Nome
  struct stat *file;       // Poneteiro das informações do file
  uid_t UID;              // User identifier                        (file.st_uid)
  off_t tam_ori;          // Tamanho original                       (Eu que guardo)
  off_t tam_atual;        // Tamanho atuaL                          (file.st_size)
  time_t data;            // Data e hora de aultima modificação     (file.st_mtime)
  long offset;            // Localização a partir do diretório      (Eu que calculo)
};

// Arquive vai ser uma lista de structs

void insere ();           // Flag: ip

void insere_comp ();      // Flag: ic

void move ();             // Flag: m

void extrai ();           // Flag: x

void remove ();           // Flag: r

void listagem ();         // Flag: c











#endif



/*  DUVIDAS ****************************************************
  Como eu guardo o meu diretório?
    Pensei em fazer uma struct que guarda apenas a lista
    Não sei que outras informações colocar  

  No enunciado diz assim :O conteúdo da área de diretório pode ser manipulado em memória RAM 
  ou em disco, a critério da equipe.
    Eu ainda não sei dizer oq vou escolher, isso fica pra depois?

  No enunciado diz assim: O conteúdo dos membros do archive deve ser manipulado diretamente em 
  disco
    Para maniupular esses dados em disco eu preciso usar aquelas funções tipo fopen, fread, fwrite, fclose...
    Mas a função fread usa um buffer para manipular as instruções, e o tamanho desse buffer é
    dinâmico de acordo com o tamanho em bytes do maior arquivo e isso eu teria que fazer em disco?
    COMO EU FAÇO ISSO EM DISCO????? 
    eu só sei fazer isso no meu programa com malloc e isso é feito no heap e não no disco, certo?

  Como eu sei que aonde começa e aonde acaba meu arquivo DIRETÓRIO dentro do arquivo?

  
  




  funcção atoi: 31 (devolve o que está escrito)
 "31"



*/