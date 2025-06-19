#include "vinac.h"




// archive: which archive is been used to move things from (binary manipulation)
// file_begin: the addres (in bytes) where the member begins
// size: number of bytes the file contains
// buffer_sz: buffer's size
// adress: the addres where member will be realocated (overwrite there)
void MoveMember (FILE* archive, long int member_begin, long int size, long int buffer_sz, long int adress) {

  // Buffer local, estático e do tamanho do maior arquivo atual
  unsigned char *buffer = malloc(sizeof (char) * buffer_sz);
  if (!buffer) {
    perror ("Erro ao alocar buffer para mover\n");
    return;
  }

  // Reads the member and save on buffer
  fseek (archive, member_begin, SEEK_SET);    // Puts the pointer at the right position (member_begin)
  fread (buffer, 1, size, archive);           // Reads 1 * size bytes from this position

  fseek (archive, adress, SEEK_SET);          // Realocate the position of the pointer at the addres i want to overwrite
  fwrite (buffer, 1, size, archive);

  if (buffer) {
    free (buffer);
    buffer = NULL;
  }
  
}

// void MoveMemberOption(FILE *archive, char **members, unsigned int num){

//   return;
// }


// archive: the file with all the information
// members: array with the binary os each member, to be converted to struct member
// num: number of new members to be insert in the array
// existent: number of current member on the array
struct member* CreateMembers (FILE* archive, char **members, unsigned int num) {
  long offset;                                                     // Cada offset de cada arquivo
  FILE *file, *binary;                                             // file: ponteiro do archive aberto; binary: ponteiro pro arquivo que quero manipular
  struct member *new = malloc (sizeof (struct member) * num);      // Vetor com os novos membros *********** DESALOCAR (segundo)

  // Abre o archive para pegar o tamanho dele
  //file = fopen (archive, "rb");
  file = archive;
  fseek (file, 0, SEEK_END);

  // Offset onde o primeiro membro novo vai ser inserido
  offset = ftell (file);                   // Tamanho do archive atual
  offset += sizeof (struct member) * num;     // Tamanho do archive atual + novos struct member
  //flose (archive);          // Só abri para pegar o tamanho dele


  // Cria todas as struct membro e preenche elas, calculando o offset corretamente
  for (unsigned int i = 0; i < num; i++) {
    binary = fopen (members[i], "rb");
    if (!binary) {
      perror ("Não consegui dar fopen members[i]\n");
      continue;
    }
    

    // Cria o ponteiro stat, o cerne do Membro
    struct stat info;                 // *********** DESALOCAR (primiero, cada um deles) 
    if (stat (members[i], &info) != 0) {
      perror ("\nErro para pegar a info STAT\n");
      return NULL;        // O que o comando continue faz??
    }
    else {

      // Preenche o membro com as informações que estão no stat
      strncpy (new[i].nome, members[i], 1024);   // Parametro 2 é escrito no Parametro 1
      new[i].UID = info.st_uid;
      new[i].tam_ori = info.st_size;
      new[i].tam_atual = info.st_size;
      new[i].data = info.st_mtime;
      new[i].offset = offset;

    }   

    // PRINT PRA DEBUG -=-=-=-=-=-
    printf ("Membro %d: %s (%ld bytes)\n", i, new[i].nome, new[i].tam_ori);


    // Aonde o próximo arquivo inserido vai começar
    offset += info.st_size;     // offset = offset + tamanho do novo arquivo
    fclose (binary);
  }
  

  return new;
}

// archive: which archive it will insert things
// members: the array containing the data of each member in it's slots
// new: number of new members to be insert
void FlatInsertion (FILE* archive, char** members, unsigned int new) {
  unsigned int num = 0; // Buffer que recebe o número de membros atualmente
  struct member *CurrentMembers, *NewMembers;
  unsigned int sum = 0, adress;   // Soma do número de membros novos com número dos anteriores; Distância que vou mover os arquivos
  int repeated = 0;
  long int maior_buff = 0, offset = 0, end = 0;                  // tamanho do arquivo inserido


  // Posiciona o ponteiro depois da assinatura (4 bytes de assinatura)
  fseek (archive, 4, SEEK_SET);

  // Lê o número de membros
  fread (&num, sizeof (unsigned int), 1, archive);         // -> O ponteiro do archive está logo apos o unsigned int
  NewMembers = CreateMembers (archive, members, new);      // Buffer que vai guardar os novos membros
  fseek (archive, 8, SEEK_SET);                            // Volta o ponteiro para onde estava


  // Se tiver membros, realoca o vetor de membros e move todos os membros pro lado direito
  if (num > 0) {
    CurrentMembers = malloc ((sizeof(struct member)) * num);       // Buffer, vetor que vai guardar todos os membros atuais  ********** DESALOCAR
    if (!CurrentMembers)
      perror ("\nNão criou o vetor de struct membros para inserção\n");

    fread (CurrentMembers, sizeof (struct member), num, archive);


    // Procuro qual seria o membro com o maior tamanho pra fazer o buffer (vou usar no MoveMember)
    for (unsigned int i = 0; i < num; i++) {      // Procura o maior tamanho dentro dos CurrentMembers
      if (CurrentMembers[i].tam_atual > maior_buff)
        maior_buff = CurrentMembers[i].tam_atual;
    }
    for (unsigned int j = 0; j < new; j++) {      // Procura o maior tamanho dentro dos NewMembers
      if (NewMembers[j].tam_atual > maior_buff)
        maior_buff = NewMembers[j].tam_atual;
    }

    fseek (archive, 0, SEEK_END);
    end = ftell (archive);


    // Esses 2 fors conferem se não tem membros repetidos, se tiver, trata a duplicada e decrementa o new
    for (unsigned int i = 0; i < num; i++) { // [i] indice do CurrentMembers
      for (unsigned int j = 0; j < new; j++) { // [j] indice do NewMembers

        // Se tiver membro repetido
        if (strcmp (CurrentMembers[i].nome, NewMembers[j].nome) == 0 ) {
          repeated++;

          // Se não forem do mesmo tamanho tem que substituir movimentando os dados
          if (CurrentMembers[i].tam_atual != NewMembers[j].tam_atual) {
            offset = CurrentMembers[i].tam_atual - NewMembers[j].tam_atual;

            // O Novo membro é maior, move os arquivos para a direita e sobreescreve
            if (offset < 0) {
              offset = -offset;  // Valor absoluto do offset
              
              // Vai lá no último membro e move a partir dele pra frente até o indice do membro exatamente igual
              for (unsigned int k = num - 1; k > i; k--) {
                MoveMember (archive, CurrentMembers[k].offset, CurrentMembers[k].tam_atual, maior_buff, CurrentMembers[k].offset + offset);
                CurrentMembers[k].offset += offset;
              }

              // Vai lá na posição e escreve o dado novo que é MAIOR
              fseek (archive, CurrentMembers[i].offset, SEEK_SET);
              NewMembers[j].offset =  CurrentMembers[i].offset;   // Atualiza o offset

              FILE* f = fopen (members[j], "rb");
              if (!f) {
                perror ("Erro ao abrir o arquivo que quero escrever\n");
                return;
              }

              char *buffer = malloc (NewMembers[j].tam_atual);
              if (!buffer) {
                perror ("Erro ao alocar buffer para escrever membro\n");
                return;
              }
              fread (buffer, NewMembers[j].tam_atual, 1, f);   // Lê o arquivo que quero escrever

              fwrite (buffer, NewMembers[j].tam_atual, 1, archive);   // Escreve o arquivo que quero escrever SEGFAULT ***********
              fseek (archive, 0, SEEK_END);
              end = ftell (archive);

              fclose (f);
              if (buffer) { 
                free (buffer);
                buffer = NULL;
              }
            }
            else {  // Move os arquivos para a esquerda
              
              // Vai no membro imediatamente depois do repetido e move ele pra esquerda, sobrescreve mesmo
              for (unsigned int k = i + 1; k < num; k++) {
                MoveMember (archive, CurrentMembers[k].offset, CurrentMembers[k].tam_atual, maior_buff, CurrentMembers[k].offset - offset);
                CurrentMembers[k].offset -= offset;
              }

              
              FILE* f = fopen (members[j], "rb"); // Abro o arquivo
              if (!f) {
                perror ("Erro ao abrir o arquivo que quero escrever\n");
                return;
              }

            
              char *buffer = malloc (NewMembers[j].tam_atual);
              if (!buffer) {
                perror ("Erro ao alocar buffer para escrever membro\n");
                return;
              }
              fread (buffer, NewMembers[j].tam_atual, 1, f);   // Lê no buffer arquivo que quero escrever

              // Vai lá na pósição e escreve o dado novo que é MENOR
              fseek (archive, CurrentMembers[i].offset, SEEK_SET);
              NewMembers[j].offset =  CurrentMembers[i].offset;   // Atualiza o offset
              fwrite (buffer, NewMembers[j].tam_atual, 1, archive);

              fclose (f);
              if (buffer) { 
                free (buffer);
                buffer = NULL;
              }

              ftruncate (fileno(archive), end - offset);

              fseek (archive, 0, SEEK_END);
              end = ftell (archive);
            }

          }
          else {  // Se forem de tamanhos iguais eu substituo direto

            FILE* f = fopen (members[j], "rb"); // Abro o arquivo
            if (!f) {
              perror ("Erro ao abrir o arquivo que quero escrever\n");
              return;
            }

          
            char *buffer = malloc (NewMembers[j].tam_atual);
            if (!buffer) {
              perror ("Erro ao alocar buffer para escrever membro\n");
              fclose (f);
              return;
            }
            fread (buffer, NewMembers[j].tam_atual, 1, f);   // Lê no buffer arquivo que quero escrever

            // Vai lá na posição e escreve o dado novo que é do mesmo tamanho
            fseek (archive, CurrentMembers[i].offset, SEEK_SET);
            NewMembers[j].offset =  CurrentMembers[i].offset;
            fwrite (buffer, NewMembers[j].tam_atual, 1, archive);

            fclose (f);
            if (buffer) { 
              free (buffer);
              buffer = NULL;
            }

          }

          CurrentMembers[i] = NewMembers[j];  // Atualiza a struct
          NewMembers[j].UID = 0;   // Flag para mostrar que o arquivo já existe e já foi tratado
          //new--;      // Diminui a quanidade de membros novos
        }
      }
    }

    // Soma to número total de membros que terá o vetor raelocado
    sum = num + (new - repeated);

    offset = sizeof (struct member) * (new - repeated);      // Quantos bytes eu vou atualizar as structs atuais
    //Move os arquivos existentes para frente  (abre espaço para atualizar o vetor de membros)

    // Se tiver offset é pq teve membro novo, dai entra no if
    if (offset) {
      for (long i = (long)num-1; i >= 0; i--) {
        adress = CurrentMembers[i].offset;
        MoveMember (archive, adress, CurrentMembers[i].tam_atual, maior_buff, adress + offset);
        CurrentMembers[i].offset += offset; // Atualiza o offset imediatamente
      }
    


      // Fazer um realoc do CurrentMembers e inserir os membros novos no final desse buffer
      CurrentMembers = realloc (CurrentMembers, sizeof(struct member) * sum);
      if (!CurrentMembers)
        perror ("\nNão realocou o vetor de struct membros\n");


      fseek (archive, 0, SEEK_END);
      // Adiciona os novos arquivos ao final do archive **** NOVO
      for (unsigned int i = 0; i < new; i++) {
        FILE *f = fopen(members[i], "rb");
        if (!f) {
            perror("Erro ao abrir o arquivo para escrita no archive");
            continue;
        }

        char *buffer = malloc(NewMembers[i].tam_atual);
        if (!buffer) {
            perror("Erro ao alocar buffer");
            fclose(f);
            continue;
        }

        fread(buffer, 1, NewMembers[i].tam_atual, f);

        if (NewMembers[i].UID != 0) {   // Se não for repetido, adiciona no final
          NewMembers[i].offset = ftell (archive);
          fwrite (buffer, NewMembers[i].tam_atual, 1, archive);
        }

        free(buffer);
        fclose(f);
      }


      unsigned int j = 0;  // Controla o indice do vetor CurrentMembers

      // Adiciona no final do vetor os novos membros que não se repetem
      for (unsigned int i = 0; i < new; i++) { // Controla o indice do vetor dos NewMembers
      
        if (NewMembers[i].UID != 0) {     // Se não for repetido, ele recebe no vetor e incrementa a posição, se não, ele pega o próximo
          CurrentMembers[num + j] = NewMembers[i];
          j++;
        }

      }

    }

    // Move o ponteiro para a posição correta e sobreescreve o novo vetor de structs membro
    fseek (archive, 8, SEEK_SET); // Move o ponteiro para a posição que começam o membros
    fwrite (CurrentMembers, sizeof (struct member), sum, archive);

    if (CurrentMembers) {
      free (CurrentMembers);
      CurrentMembers = NULL;
    }

  } 
  else {  // Caso meu archive não tenha membro nenhum ainda
    // Ponteiro do archive está logo após do unsigned int

    fseek  (archive, 8, SEEK_SET);
    fwrite (NewMembers, sizeof (struct member), new, archive);


    fseek (archive, 0, SEEK_END);

    // Adiciona os novos arquivos ao final do archive
    for (unsigned int i = 0; i < new; i++) {
      NewMembers[i].offset = ftell(archive); // Garante que está sendo escrito no offset certo

      FILE *f = fopen(members[i], "rb");
      if (!f) {
          perror("Erro ao abrir o arquivo para escrita no archive");
          continue;
      }

      char *buffer = malloc(NewMembers[i].tam_atual);
      if (!buffer) {
          perror("Erro ao alocar buffer");
          fclose(f);
          continue;
      }

      fread(buffer, 1, NewMembers[i].tam_atual, f);

      NewMembers[i].offset = ftell (archive); // Garante que está sendo escrito no offset certo
      fwrite (buffer, NewMembers[i].tam_atual, 1, archive);

      free(buffer);
      fclose(f);
    }

    sum = new; // Vai escreve e quantidade e membros novos
  }

  fseek (archive, 4, SEEK_SET);                      // Coloco o ponteiro no início do número de membro mais uma vez
  fwrite (&sum, sizeof (unsigned int), 1, archive);   // Sobre escrevo o novo número na posição correta
  

  if (NewMembers) {
    free (NewMembers);
    NewMembers = NULL;
  }
  
  printf ("chegou no fim do FLatInsertion!!!!\n");
}


void list (FILE* archive, unsigned int existent) {
  if (existent == 0) {
    printf ("Archive vazio! Não tem membros\n");
    return;
  }

  if (!archive) {
    perror ("Archive inválido!\n");
    return;
  }

  struct member m;
  fseek (archive, 8, SEEK_SET);   // Posiciona o ponteiro depois da assinatura e o inteiro (8 bytes)

  for (unsigned int i = 0; i < existent; i++) {
    fread (&m, sizeof (struct member), 1, archive);  
    // Depois de ler o ponteiro também andou junto 1 struct member pra frente
    
    printf ("\n============= Arquivo %d =============\n", i+1);
    printf ("    Nome: %s\n", m.nome);
    printf ("    UID: %u\n", (unsigned int) m.UID);
    printf ("    Tamanho original: %ld\n", (long) m.tam_ori);
    printf ("    Tamanho atual: %ld\n", (long) m.tam_atual);
    printf ("    Última modificação: %s", ctime(&m.data));
    printf ("    Offset do arquivo: %ld\n", (long)m.offset);
  }
  
}


struct member* CreateCompressedMembers (FILE* archive, char **members, unsigned int num, char **comp_members) {
  long offset, csz = 0;                                           // Cada offset de cada arquivo
  FILE *file, *binary;                                             // file: ponteiro do archive aberto; binary: ponteiro pro arquivo que quero manipular
  unsigned char *flat = NULL, *compressed = NULL;
  struct member *new = malloc (sizeof (struct member) * num);      // Vetor com os novos membros *********** DESALOCAR (segundo)
  
  
  if (!new) {
    perror ("New não foi alocado no CreateCompressedMembers\n");
    return NULL;
  }


  // Abre o archive para pegar o tamanho dele
  //file = fopen (archive, "rb");
  file = archive;
  fseek (file, 0, SEEK_END);

  // Offset onde o primeiro membro novo vai ser inserido
  offset = ftell (file);                   // Tamanho do archive atual
  offset += sizeof (struct member) * num;     // Tamanho do archive atual + novos struct member
  //fclose (archive);          // Só abri para pegar o tamanho dele


  // Cria todas as struct membro e preenche elas, calculando o offset corretamente
  for (unsigned int i = 0; i < num; i++) {
    binary = fopen (members[i], "rb");
    if (!binary) {
      perror ("Não consegui dar fopen members[i]\n");
      continue;
    }

    fseek (binary, 0, SEEK_END);
    long insize = ftell (binary);
    rewind (binary);

    flat = malloc (insize);
    fread (flat, 1, insize, binary);
    rewind (binary);

    compressed = malloc (insize * 2);
    csz = LZ_Compress (flat, compressed, insize);
    if (csz <= 0) {
      perror ("Erro na compressão do LZ_Compress\n");
    }

    // fwrite (compressed, csz, 1, binary);  // Escreve o arquivo comprimido no lugar do arquivo original
    fclose (binary);

    if (csz >= insize) {
      csz = insize;
      memcpy(compressed, flat, insize);
    }

    if (flat) {
      free (flat);
      flat = NULL;
    }

    if (compressed) {
      comp_members[i] = malloc (sizeof (char*) * csz);
      if (!comp_members[i]) {
        perror ("Erro ao alocar comp_members[i]\n");
        return NULL;
      }
      memcpy (comp_members[i], compressed, csz);
    }
    else {
      comp_members[i] = NULL;
    }

    // Cria o ponteiro stat, o cerne do Membro
    struct stat info;                 // *********** DESALOCAR (primiero, cada um deles) 
    if (stat (members[i], &info) != 0) {
      perror ("\nErro para pegar a info STAT\n");
      return NULL;        // O que o comando continue faz??
    }
    else {

      // Preenche o membro com as informações que estão no stat
      strncpy (new[i].nome, members[i], 1024);   // Parametro 2 é escrito no Parametro 1
      new[i].UID = info.st_uid;
      new[i].tam_ori = info.st_size;
      new[i].tam_atual = csz;       // Tamanho comprimido fica aqui dentro
      new[i].data = info.st_mtime;
      new[i].offset = offset;
    }   


    if (compressed) {
      free (compressed);
      compressed = NULL;
    }

    // PRINT PRA DEBUG -=-=-=-=-=-
    printf ("Membro %d: %s (%ld bytes)\n", i, new[i].nome, new[i].tam_atual);


    // Aonde o próximo arquivo inserido vai começar
    offset += new[i].tam_atual;     // offset = offset + tamanho do novo arquivo
  }
  
  return new;
}


void CompressedInsertion (FILE* archive, char** members, unsigned int new) {
  unsigned int num = 0; // Buffer que recebe o número de membros atualmente
  struct member *CurrentMembers, *NewMembers;
  unsigned int sum, adress;   // Soma do número de membros novos com número dos anteriores; Distância que vou mover os arquivos
  int repeated = 0;
  
  // FILE *f;                   // Ponteiro auxiliar para guardar o arquivo
  long end = 0;                  // tamanho do arquivo inserido
  long int maior_buff = 0, offset = 0;                  // tamanho do arquivo inserido
  char **comp_mem = NULL;         // Vetor de ponteiros para strings são arquivos comprimidos


  // Posiciona o ponteiro depois da assinatura (4 bytes de assinatura)
  fseek (archive, 4, SEEK_SET);

  comp_mem = malloc (sizeof (char*) * new); // Aloca o vetor de ponteiros

  // Lê o número de membros
  fread (&num, sizeof (unsigned int), 1, archive);         // -> O ponteiro do archive está logo apos o unsigned int
  NewMembers = CreateCompressedMembers (archive, members, new, comp_mem);      // Buffer que vai guardar os novos membros
  fseek (archive, 8, SEEK_SET);                            // Volta o ponteiro para onde estava


  // Se tiver membros, realoca o vetor de membros e move todos os membros pro lado direito
  if (num > 0) {
    CurrentMembers = malloc ((sizeof(struct member)) * num);       // Buffer, vetor que vai guardar todos os membros atuais  ********** DESALOCAR
    if (!CurrentMembers)
      perror ("\nNão criou o vetor de struct membros para inserção\n");

    fread (CurrentMembers, sizeof (struct member), num, archive);


    // Procuro qual seria o membro com o maior tamanho pra fazer o buffer (vou usar no MoveMember)
    for (unsigned int i = 0; i < num; i++) {      // Procura o maior tamanho dentro dos CurrentMembers
      if (CurrentMembers[i].tam_atual > maior_buff)
        maior_buff = CurrentMembers[i].tam_atual;
    }
    for (unsigned int j = 0; j < new; j++) {      // Procura o maior tamanho dentro dos NewMembers
      if (NewMembers[j].tam_atual > maior_buff)
        maior_buff = NewMembers[j].tam_atual;
    }

    fseek (archive, 0, SEEK_END);
      end = ftell (archive);

    // Esses 2 fors conferem se não tem membros repetidos, se tiver, trata a duplicada e decrementa o new
    for (unsigned int i = 0; i < num; i++) { // [i] indice do CurrentMembers
      for (unsigned int j = 0; j < new; j++) { // [j] indice do NewMembers

        // Se tiver membro repetido
        if (strcmp (CurrentMembers[i].nome, NewMembers[j].nome) == 0 ) {
          repeated++;

          // Se não forem do mesmo tamanho tem que substituir movimentando os dados
          if (CurrentMembers[i].tam_atual != NewMembers[j].tam_atual) {
            offset = CurrentMembers[i].tam_atual - NewMembers[j].tam_atual;

            // O Novo membro é maior, move os arquivos para a direita e sobreescreve
            if (offset < 0) {
              offset = -offset;  // Valor absoluto do offset
              
              // Vai lá no último membro e move a partir dele pra frente até o indice do membro exatamente igual
              for (unsigned int k = num - 1; k > i; k--) {
                MoveMember (archive, CurrentMembers[k].offset, CurrentMembers[k].tam_atual, maior_buff, CurrentMembers[k].offset + offset);
                CurrentMembers[k].offset += offset;
              }

            
              char *buffer = malloc (NewMembers[j].tam_atual);
              if (!buffer) {
                perror ("Erro ao alocar buffer para escrever membro\n");
                return;
              }

              buffer = comp_mem[j];

              // Vai lá na posição e escreve o dado novo que é MAIOR
              fseek (archive, CurrentMembers[i].offset, SEEK_SET);
              NewMembers[j].offset =  CurrentMembers[i].offset;   // Atualiza o offset
              fwrite (buffer, NewMembers[j].tam_atual, 1, archive);

              fseek (archive, 0, SEEK_END);
              end = ftell (archive);

              if (buffer) { 
                free (buffer);
                buffer = NULL;
              }

            }
            else {  // Move os arquivos para a esquerda
              
              // Vai no membro imediatamente depois do repetido e move ele pra esquerda, sobrescreve mesmo
              for (unsigned int k = i + 1; k < num; k++) {
                MoveMember (archive, CurrentMembers[k].offset, CurrentMembers[k].tam_atual, maior_buff, CurrentMembers[k].offset - offset);
                CurrentMembers[k].offset -= offset;
              }


              char *buffer = malloc (NewMembers[j].tam_atual);
              if (!buffer) {
                perror ("Erro ao alocar buffer para escrever membro\n");
                return;
              }

              buffer = comp_mem[j];

              // Vai lá na pósição e escreve o dado novo que é MENOR
              fseek (archive, CurrentMembers[i].offset, SEEK_SET);
              NewMembers[j].offset =  CurrentMembers[i].offset;   // Atualiza o offset
              fwrite (buffer, NewMembers[j].tam_atual, 1, archive);

              if (buffer) { 
                free (buffer);
                buffer = NULL;
              }

              fseek (archive, 0, SEEK_END);
              end = ftell (archive);

              ftruncate (fileno(archive), end - offset);

              fseek (archive, 0, SEEK_END);
              end = ftell (archive);
            }

          }
          else {  // Se forem de tamanhos iguais eu substituo direto
            // Vai lá na posição e escreve o dado novo que é do mesmo tamanho

            
            char *buffer = malloc (NewMembers[j].tam_atual);
            if (!buffer) {
              perror ("Erro ao alocar buffer para escrever membro\n");
              return;
            }

            buffer = comp_mem[j];

            fseek (archive, CurrentMembers[i].offset, SEEK_SET);
            NewMembers[j].offset =  CurrentMembers[i].offset;
            fwrite (buffer, NewMembers[j].tam_atual, 1, archive);

            if (buffer) { 
              free (buffer);
              buffer = NULL;
            }

          }

          CurrentMembers[i] = NewMembers[j];  // Atualiza a struct
          NewMembers[j].UID = 0;   // Flag para mostrar que o arquivo já existe e já foi tratado
          //new--;      // Diminui a quanidade de membros novos
        }
      }
    }

    // Tira o número de membros que foram reptidos e já foram tratados
    //new -= repeated;    // New pode dar 0 se eu apenas substituir

    // Soma to número total de membros que terá o vetor raelocado
    sum = num + (new - repeated);

    offset = sizeof (struct member) * (new - repeated);      // Quantos bytes eu vou atualizar as structs atuais
    //Move os arquivos existentes para frente  (abre espaço para atualizar o vetor de membros)

    // Se tiver offset é pq teve membro novo, dai entra no if
    if (offset) {
      for (long i = (long)num-1; i >= 0; i--) {
        adress = CurrentMembers[i].offset;
        MoveMember (archive, adress, CurrentMembers[i].tam_atual, maior_buff, adress + offset);
        CurrentMembers[i].offset += offset; // Atualiza o offset imediatamente
      }
    


      // Fazer um realoc do CurrentMembers e inserir os membros novos no final desse buffer
      CurrentMembers = realloc (CurrentMembers, sizeof(struct member) * sum);
      if (!CurrentMembers)
        perror ("\nNão realocou o vetor de struct membros\n");


      fseek (archive, 0, SEEK_END);

      // Adiciona os novos arquivos ao final do archive
      for (unsigned int i = 0; i < new; i++) {

        char *buffer = malloc (NewMembers[i].tam_atual);
        if (!buffer) {
          perror ("Erro ao alocar buffer para escrever membro\n");
          return;
        }

        buffer = comp_mem[i];

        if (NewMembers[i].UID != 0) {   // Se não for repetido, adiciona no final
          NewMembers[i].offset = ftell (archive);
          fwrite (comp_mem[i], NewMembers[i].tam_atual, 1, archive);
        }

        if (buffer) { 
          free (buffer);
          buffer = NULL;
        }
      }


      unsigned int j = 0;  // Controla o indice do vetor CurrentMembers

      // Adiciona no final do vetor os novos membros que não se repetem
      for (unsigned int i = 0; i < new; i++) { // Controla o indice do vetor dos NewMembers
      
        if (NewMembers[i].UID != 0) {     // Se não for repetido, ele recebe no vetor e incrementa a posição, se não, ele pega o próximo
          CurrentMembers[num + j] = NewMembers[i];
          j++;
        }

      }

    }

    // Move o ponteiro para a posição correta e sobreescreve o novo vetor de structs membro
    fseek (archive, 8, SEEK_SET); // Move o ponteiro para a posição que começam o membros
    fwrite (CurrentMembers, sizeof (struct member), sum, archive);

    if (CurrentMembers) {
      free (CurrentMembers);
      CurrentMembers = NULL;
    }

  } 
  else {  // Caso meu archive não tenha membro nenhum ainda
    // Ponteiro do archive está logo após do unsigned int
    fwrite (NewMembers, sizeof (struct member), new, archive);


    fseek (archive, 0, SEEK_END);

    // Adiciona os novos arquivos ao final do archive **** NOVO
    for (unsigned int i = 0; i < new; i++) {
      NewMembers[i].offset = ftell (archive); // Garante que está sendo escrito no offset certo
      fwrite (comp_mem[i], NewMembers[i].tam_atual, 1, archive);
    }

    sum = new; // Vai escreve e quantidade e membros novos
  }

  fseek (archive, 4, SEEK_SET);                      // Coloco o ponteiro no início do número de membro mais uma vez
  fwrite (&sum, sizeof (unsigned int), 1, archive);   // Sobre escrevo o novo número na posição correta
  

  if (NewMembers) {
    free (NewMembers);
    NewMembers = NULL;
  }

  printf ("chegou no fim do CompressedInsertion!!!!\n");
}





// members_to_remove: vetor de strings com nomes a remover
// n_remove: quantos nomes no vetor
// members_to_remove: vetor de strings com nomes a remover
// n_remove: quantos nomes no vetor
void RemoveMembers(FILE *archive, char **targets, unsigned int n) {
  long end = 0, offset = 0;
  long int maior_buff = 0;
  unsigned int ori_sz;   // Quantidade de membros atualmente
  struct member *old_tbl, *new_tbl;

  // Se o número de membros for menor que 1, remove todos
  if (!targets || n < 1) {
    printf ("Tem certeza que quer remover todos os membros?\n");
    printf ("  s: Sim\n");
    printf ("  n: Não\n");

    char aux = 0;
    scanf ("%c", &aux);
    if (aux == 's') {
      ftruncate (fileno(archive), 8);
      fseek (archive, 4, SEEK_SET);
      unsigned int a = 0;
      fwrite (&a, sizeof a, 1, archive);
      rewind (archive);

      printf ("Removeu tudo\n");
    }
    return;
  }

  // Lê o número da quantidade de itens que tem dentro do archive
  fseek (archive, 4, SEEK_SET);
  if (fread (&ori_sz, sizeof ori_sz, 1, archive) != 1) {
    perror ("Erro ao ler o inteiro no inicion do arquivo\n");
    return;
  }

  if (ori_sz == 0) {
    printf ("Não tem membros para remover\n");
    return;
  }

  // Aloca um vetor com os membros antigos e preenche eles
  if (ori_sz) {
    old_tbl = malloc (sizeof (struct member) * ori_sz);
    if (!old_tbl) {
      perror ("Erro no malloc de old_tbl\n");
      return;
    }

    fseek (archive, 8, SEEK_SET);
    fread (old_tbl, sizeof (struct member), ori_sz, archive);
  }

  // Procuro qual seria o membro com o maior tamanho pra fazer o buffer (vou usar no MoveMember)
  for (unsigned int i = 0; i < ori_sz; i++) {
    if (old_tbl[i].tam_atual > maior_buff)
      maior_buff = old_tbl[i].tam_atual;
  }

  // Guarda o endereço do fim para truncar lá depois
  fseek (archive, 0, SEEK_END);
  end = ftell (archive);

  unsigned int rem_mem = 0;   // Número de membros removidos (nem sempre todos os membros que pedem para ser removidos estão no archive)

  // É nesse for que são movidos os arquivos binários, o diretório permanece inalterado
  // Vai pegar cada posição do vetor de structs e dizer se é aquele membro ou não
  // i: indice de qual binário está sendo analisado
  for (unsigned int i = 0; i < ori_sz; i++) {
    // j: o indice do membro do target
    // diffrent: flag pra saber ser os nomes são diferentes ou não
    unsigned int j = 0, diffrent = 1;
    while (j < n && diffrent) {
      diffrent = strcmp (targets[j], old_tbl[i].nome);    // Se retornar 0, as strings são iguais, dai sai do laço
      j++;
    }
    j--;  // Número certo do indice

    // Se achou o target, sobrescreve movendo todos os outros
    if (diffrent == 0 && old_tbl[i].UID != 0) { 
      offset = old_tbl[i].tam_atual;    // Quanto que cada  binário vai se mover pra esquerda
      old_tbl[i].UID = 0;   // Marcação que foi removido
      rem_mem++;            // Removeu +1

      // K começa desde o próximo membro depois do target
      // Esse for move todos os binários sobreescrevendo o arquivo target
      for (unsigned int k = i + 1; k < ori_sz; k++) {
        // Move cada membro para trás
        MoveMember (archive, old_tbl[k].offset, old_tbl[k].tam_atual, maior_buff, old_tbl[k].offset - offset);
        old_tbl[k].offset -= offset;
      }

      // Todos os arquivos já foram movidos, agora precisa tirar um lixo de arquivo duplicado lá do final
      // No caso em que o target for o último, a remoção dele vai ser tratada aqui
      ftruncate (fileno(archive), end - offset);

      // Guarda o novo endereço do fim (depois do truncamento)
      fseek (archive, 0, SEEK_END);
      end = ftell (archive);
    }
  }

  // A novo tamanho do vetor de membros
  unsigned int new_sz = ori_sz - rem_mem;
  
  // Aloca a nova tabela para ser preenchida como os membros que permaneceram
  new_tbl = malloc (sizeof (struct member) * new_sz);
  if (!new_tbl) {
    perror ("Erro no malloc de new_tbl\n");
    return;
  }

  // i: indice da tabela nova
  // j: indice da tabela antiga
  // k: indice do target que eu estou comparando
  unsigned int j = 0;
  // unsigned int remove;        // Flag que controla se remove ou não

  // Aqui neste for eu vou preencher a nova tabela
  for (unsigned int i = 0; i < ori_sz; i++) {
    if (old_tbl[i].UID != 0) {    // Essa marcação no UID foi feita no for anterior aonde eu removi os aquivos
      new_tbl[j] = old_tbl[i];
      j++;
    }
  }

  maior_buff = 0;
  // Procuro qual seria o membro com o maior tamanho pra fazer o buffer (vou usar no MoveMember)
  for (unsigned int i = 0; i < new_sz; i++) {
    if (new_tbl[i].tam_atual > maior_buff)
      maior_buff = new_tbl[i].tam_atual;
  }

  // Daqui pra frente eu vou mover os membros offset bytes para a esquerda
  // Eu quero remover o número de membros está sobrando
  offset = rem_mem * sizeof (struct member);

  // Guarda o endereço do fim para truncar lá depois
  fseek (archive, 0, SEEK_END);
  end = ftell (archive);

  // Move todos os binários para a esquerda (sobreescrevendo o diretório antigo) e atualiza o offset deles
  for (unsigned int i = 0; i < new_sz; i++) {
    MoveMember (archive, new_tbl[i].offset, new_tbl[i].tam_atual, maior_buff, new_tbl[i].offset - offset);
    new_tbl[i].offset -= offset;
  }

  // Todos os arquivos já foram movidos, agora precisa tirar um lixo de arquivo duplicado lá do final
  // No caso em que o target for o último, a remoção dele vai ser tratada aqui
  ftruncate (fileno(archive), end - offset);

  // Sobrescrevo a nova tabela
  fseek (archive, 8, SEEK_SET);
  fwrite (new_tbl, sizeof (struct member), new_sz, archive);
  
  // Libera as memórias alocadas
  if (old_tbl) {
    free (old_tbl);
    old_tbl = NULL;
  }

  if (new_tbl) {
    free (new_tbl);
    new_tbl = NULL;
  }
  
  // Escreve o novo número de membros presentes no diretório
  fseek (archive, 4, SEEK_SET);
  fwrite (&new_sz, sizeof (unsigned int), 1, archive);

  rewind (archive); 
}


void MemberExtract (FILE *archive, char **targets, unsigned int n) {
  unsigned int num = 0;   

  fseek (archive, 4, SEEK_SET);
  fread (&num, sizeof (unsigned int), 1, archive);         // -> O ponteiro do archive está logo apos o unsigned int
  if (num == 0) {
    printf ("Não tem membros para extrair\n");
    return;
  }

  if (n>0) {
    for (unsigned int i = 0; i < n; i++) {
      for (unsigned int j = 0; j < num; j++) {
        struct member m;
        fseek (archive, 8 + (j * sizeof (struct member)), SEEK_SET);   // Move o ponteiro para a posição do membro j
        fread (&m, sizeof (struct member), 1, archive);                // Lê o membro j

        if (strcmp (m.nome, targets[i]) == 0) {   // Se o nome do membro for igual ao nome que quero extrair
          FILE *f = fopen (targets[i], "wb");     // Abre o arquivo para escrever
          if (!f) {
            perror ("Erro ao abrir o arquivo para extrair\n");
            return;
          }

          unsigned char *buffer = malloc (m.tam_atual);
          if (!buffer) {
            perror ("Erro ao alocar buffer para extrair membro\n");
            fclose(f);
            return;
          }

          fseek (archive, m.offset, SEEK_SET);    // Move o ponteiro para a posição do membro j
          fread (buffer, m.tam_atual, 1, archive);   // Lê o membro j

          // Ve se o membro está comprimido ou não
          if (m.tam_atual != m.tam_ori) {
            unsigned char *flat_buf = malloc (m.tam_ori);
            if (!flat_buf) {
              perror ("Erro ao alocar buffer para descomprimir membro\n");
              fclose(f);
              free(buffer);
              return;
            }
            LZ_Uncompress (buffer, flat_buf, m.tam_atual);  // Descomprime o membro
            m.tam_atual = m.tam_ori;  // Atualiza o tamanho atual para o tamanho original
            free(buffer);  // Libera o buffer comprimido
            buffer = flat_buf;  // Atualiza o buffer para o descomprimido

          }

          fwrite (buffer, m.tam_atual, 1, f);     // Escreve no arquivo que foi aberto

          fclose(f);
          free(buffer);
        }
      }
    }
  }
  else {  // Se não passar nenhum nome, extrai todos os membros
    for (unsigned int i = 0; i < num; i++) {
      struct member m;
      fseek (archive, 8 + (i * sizeof (struct member)), SEEK_SET);   // Move o ponteiro para a posição do membro i
      fread (&m, sizeof (struct member), 1, archive);                // Lê o membro i

      FILE *f = fopen (m.nome, "wb");     // Abre o arquivo para escrever
      if (!f) {
        perror ("Erro ao abrir o arquivo para extrair\n");
        return;
      }

      unsigned char *buffer = malloc (m.tam_atual);
      if (!buffer) {
        perror ("Erro ao alocar buffer para extrair membro\n");
        fclose(f);
        return;
      }

      fseek (archive, m.offset, SEEK_SET);    // Move o ponteiro para a posição do membro i
      fread (buffer, m.tam_atual, 1, archive);   // Lê o membro i

      // Ve se o membro está comprimido ou não
      if (m.tam_atual != m.tam_ori) {
        unsigned char *flat_buf = malloc (m.tam_ori);
        if (!flat_buf) {
          perror ("Erro ao alocar buffer para descomprimir membro\n");
          fclose(f);
          free(buffer);
          return;
        }
        LZ_Uncompress (buffer, flat_buf, m.tam_atual);  // Descomprime o membro
        m.tam_atual = m.tam_ori;  // Atualiza o tamanho atual para o tamanho original
        free(buffer);  // Libera o buffer comprimido
        buffer = flat_buf;  // Atualiza o buffer para o descomprimido
        
      }

      fwrite (buffer, m.tam_atual, 1, f);     // Escreve no arquivo que foi aberto

      fclose(f);
      free(buffer);
    }
  }
}


// Move um membro de uma posição para outra dentro do arquivo archive
// archive: arquivo aberto
// member_begin: offset de onde começa o membro a ser movido
// size: tamanho do membro a ser movido (em bytes)
// buffer_sz: tamanho do maior membro (para o buffer temporário)
// adress: novo offset para onde o membro será movido
void MoveMember(FILE* archive, long int member_begin, long int size, long int buffer_sz, long int adress) {
    // Aloca buffer temporário do tamanho do maior membro
    unsigned char *buffer = malloc(buffer_sz);
    if (!buffer) {
        perror("Erro ao alocar buffer para mover membro");
        return;
    }

    // Lê o conteúdo do membro a ser movido
    fseek(archive, member_begin, SEEK_SET);
    size_t lidos = fread(buffer, 1, size, archive);
    if (lidos != size) {
        perror("Erro ao ler membro para mover");
        free(buffer);
        return;
    }

    // Escreve o conteúdo na nova posição
    fseek(archive, adress, SEEK_SET);
    size_t escritos = fwrite(buffer, 1, size, archive);
    if (escritos != size) {
        perror("Erro ao escrever membro movido");
    }

    free(buffer);
}


