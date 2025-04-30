#include <stdio.h>
#include <stdlib.h>
#include <string.h>




int main () {

  // Cria um arquivo texto, caso exista ele será destruido
  FILE* archive;
  char option [3];                        // 2 + 1 incluindo o \0 
  char name [1025];                       // 1024 + 1 incluindo o \0

  void *result;

  // Leio a opção e o nome do arquivo
  scanf ("%s %1025s", option, name);

  char option [2];

  // Compara se as duas strings são iguais, se forem, entra no loop
  while (strcmp (option, "exit\n") != 0) {
    if (option == "p") {                // Insere arquivos planos
      printf ("Função P selecionada\n");
      
      archive = fopen (name, "a+");
      if (!archive) {
        perror ("Erro ao criar archive\n");
        exit (1);
      }
    


    } else if (option == "i") {         // Insere arquivos compactados
      printf ("Função I selecionada\n");

      archive = fopen (name, "a+");
      if (!archive) {
        perror ("Erro ao criar archive\n");
        exit (1);
      }
      
    } else if (option == "m") {
      printf ("Função M selecionada\n");
      
      archive = fopen (name, "a+");
      if (!archive) {
        perror ("Erro ao criar archive\n");
        exit (1);
      }

    } else if (option == "x") {
      printf ("Função X selecionada\n");

      archive = fopen (name, "a+");
      if (!archive) {
        perror ("Erro ao criar archive\n");
        exit (1);
      }

      

    } else if (option == "r") {
      printf ("Função R selecionada\n");

      archive = fopen (name, "a+");
      if (!archive) {
        perror ("Erro ao criar archive\n");
        exit (1);
      }

    } else if (option == "c") {
      printf ("Função C selecionada\n");

      archive = fopen (name, "a+");
      if (!archive) {
        perror ("Erro ao criar archive\n");
        exit (1);
      }

    }


  }


  printf ("Fim!\n");


  return 0;
}

/*


switch
  ip

  ic

  m

  x

  r

  c




*/