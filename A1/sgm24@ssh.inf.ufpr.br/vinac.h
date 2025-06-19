#ifndef __ARCHIVE__
#define __ARCHIVE__

#ifndef _vinac_h_
#define _vinac_h_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>     // Para usar o Stat
#include <unistd.h>       // Para usar funções como access(), read(), write(), close(), truncate () etc.
#include <sys/types.h>    // Para usar os types que o Stat tem, tipo o off_t
#include <time.h>         // Para usar funções como ctime(), localtime(), strftime(), etc.
#include <stdbool.h>
#include "lz.h"


struct member {
  char nome [1024];       // Nome
  uid_t UID;              // User identifier                        (file.st_uid)
  off_t tam_ori;          // Tamanho original                       (Eu que guardo)
  off_t tam_atual;        // Tamanho atuaL                          (file.st_size)
  time_t data;            // Data e hora de aultima modificação     (file.st_mtime)
  long offset;            // Localização a partir do diretório      (Eu que calculo)
};

// Arquive vai ser uma lista de structs

// Creates a vecto fullfiled of struct memebers filled with **members info
struct member* CreateMembers (FILE* archive, char **members, unsigned int num);

void MoveMember (FILE* archive, long int member_begin, long int size, long int buffer_sz, long int adress);

// void descomprime ();


// Flag -p
void FlatInsertion (FILE* archive, char** members, unsigned int new);

// Flag -i
void CompressedInsertion (FILE* archive, char** members, unsigned int new);

// Flag: -m
void MoveMember(FILE* archive, long int member_begin, long int size, long int buffer_sz, long int adress);

// Flag: -x
void MemberExtract (FILE* archive, char** members, unsigned int num);

// Flag: -r
void RemoveMembers(FILE *archive, char **targets, unsigned int n);

// Flag: -c
void list (FILE* archive, unsigned int num);











#endif
#endif