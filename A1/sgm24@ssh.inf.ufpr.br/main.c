#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "vinac.h"
#include "lz.h"

// Defino o nome do arquivo que quero manipular
// #define ARQUIVO "archiver.vc"    <-- isso se torna inútil com o argv[1]



// argc diz o número de parãmetros
// argv é o vetor que guarda os parâmetros inseridos
int main (int argc, char **argv) {

  char header [4];            // Buffer para assinatura
  unsigned int existent = 0;  // Número existente de arquivos
  unsigned int num = 0;       // Número de membros que quero inserir
  char *option, *archive, **members;

  // Verifica se foram escritos
  if (argc < 3) {
    printf ("Número de parâmetros inválido!\n");
    exit(1);
  }

  // O argv[0] é o nome do arquivo executável, não deve ser considerado
  option = argv[1];
  archive = argv[2];
  num += argc - 3;        // Os membros que foram escritos começam em argv[3] e vão até argv [argc-1] (*Tem funções que não passa membro nenhum*)


  //  conteúdo anterior do arquivo é preservado e o ponteiro é posicionado no início do arquivo (leitura e escrita)
  FILE *file = fopen (archive, "r+b");
  

  // IF caso o arquino não exista
  if (!file) {
    // Assinatura pra GARANTIR que criei o arquivo próxima vez que eu for ler
    strcpy (header, "SGM");      // sgm + \0
    file = fopen (archive, "w+b");
    fwrite (&header, sizeof (char), 4, file);
    fwrite (&existent, sizeof (unsigned int), 1, file);
  }
  else { // Se existe, leia a assinatura dele
    fread (&header, sizeof(char), 4, file);
  }
  

  // Testa se a assinatura ilda é igual a SGM, se for, retorna 0
  if (strcmp (header, "SGM") != 0) {
    printf ("Sei lá que arquivo é esse\n");
  } 
  else {
  // Manda o ponteiro pro início do arquivo depois da assinatura (4 bytes)
  fseek (file, 4, SEEK_SET);
  fread (&existent, sizeof (unsigned int), 1, file);    // Lé a quantidade de membros existentes (+ 4 bytes)
  }

  // Coloca os memebros lidos no **members (vetor)
  if (num > 0 ) { 

    // Aloca um vetor com o número de membros lidos
    members = malloc(sizeof(char*) * (num));
    for(int i=3; i<argc; i++)
      members[i-3] = argv[i];   // O vetor de membros recebe cada membro
  }
  else
    members = NULL;



  // strcpy retorna 0 ser as strings forem iguais
  if (strcmp (option, "-p") == 0) {       // Insere plano
    
    printf ("\n");
    // Se existirem membros, chamo a inserção
    if (members)
      FlatInsertion (file, members, num);
  } 
  else if (strcmp (option, "-i") == 0) {  // Insere compactado
    CompressedInsertion (file, members, num);
  } 
  else if (strcmp (option, "-m") == 0) {  // Move o membro
    MoveMember(archive, long int member_begin, long int size, long int buffer_sz, long int adress);
  } 
  else if (strcmp (option, "-x") == 0) {
    MemberExtract (archive, members, num);
  }
  else if (strcmp (option, "-r") == 0) {
    RemoveMembers (file, members, num);
  }
  else if (strcmp (option, "-c") == 0) {
      if (!file)
        perror ("File inválido!\n");
      list (file, existent);
  } 
  else
    printf ("Opção inexistente!\n");


  if (members != NULL)
    free(members);


  printf ("\nFim!\n");


  return 0;
}