#include "vinac.h"

// archive: the file with all the information
// members: array with the binary os each member, to be converted to struct member
// num: number of new members to be insert in the array
// existent: number of current member on the array
struct member* CreateCompressedMembers (FILE* archive, char **members, unsigned int num) {
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

    long insize = ftell (binary);
    rewind (binary);

    char *flat = malloc (insize);
    fread (flat, 1, insize, binary);
    fclose (binary);

    char *compressed = malloc (insize * 2);
    long csz = LZ_Compress (flat, compressed, insize);

    if (csz >= insize) {
      csz = insize;
      memcpy(compressed, flat, insize);
    }
    free (flat);

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

    // Guardo o ponteiro para o arquivo comprimido dentro dos próprios members
    members[i] = compressed;

    free (compressed);

    // PRINT PRA DEBUG -=-=-=-=-=-
    printf ("Membro %d: %s (%ld bytes)\n", i, new[i].nome, new[i].tam_atual);


    // Aonde o próximo arquivo inserido vai começar
    offset += new[i].tam_atual;     // offset = offset + tamanho do novo arquivo
    fclose (binary);
  }
  

  return new;
}



// archive: which archive it will insert things
// members: the array containing the data of each member in it's slots
// new: number of new members to be insert
void CompressedInsertion (FILE* archive, char** members, unsigned int new) {
  unsigned int num = 0; // Buffer que recebe o número de membros atualmente
  struct member *CurrentMembers, *NewMembers;
  unsigned int sum, adress;   // Soma do número de membros novos com número dos anteriores; Distância que vou mover os arquivos
  
  // FILE *f;                   // Ponteiro auxiliar para guardar o arquivo
  long tam = 0;                  // tamanho do arquivo inserido
  long int maior_buff = 0, offset = 0;                  // tamanho do arquivo inserido


  // Posiciona o ponteiro depois da assinatura (4 bytes de assinatura)
  fseek (archive, 4, SEEK_SET);

  // Lê o número de membros
  fread (&num, sizeof (unsigned int), 1, archive);         // -> O ponteiro do archive está logo apos o unsigned int
  NewMembers = CreateCompressedMembers (archive, members, new);      // Buffer que vai guardar os novos membros
  fseek (archive, 8, SEEK_SET);                            // Volta o ponteiro para onde estava


  // Se tiver membros, realoca o vetor de membros e move todos os membros pro lado direito
  if (num > 0) {
    CurrentMembers = malloc ((sizeof(struct member)) * num);       // Buffer, vetor que vai guardar todos os membros atuais  ********** DESALOCAR
    if (!CurrentMembers)
      perror ("\nNão criou o vetor de struct membros para inserção\n");

    fread (CurrentMembers, sizeof (struct member), num, archive);


    // Procuro qual seria o membro com o maior tamanho pra fazer o buffer (vou usar no MoveMember)
    for (unsigned int i = 0; i < tam; i++) {      // Procura o maior tamanho dentro dos CurrentMembers
      if (CurrentMembers[i].tam_atual > maior_buff)
        maior_buff = CurrentMembers[i].tam_atual;
    }
    for (unsigned int j = 0; j < new; j++) {      // Procura o maior tamanho dentro dos NewMembers
      if (NewMembers[j].tam_atual > maior_buff)
        maior_buff = NewMembers[j].tam_atual;
    }

    // Esses 2 fors conferem se não tem membros repetidos, se tiver, trata a duplicada e decrementa o new
    for (unsigned int i = 0; i < num; i++) { // [i] indice do CurrentMembers
      for (unsigned int j = 0; j < new; j++) { // [j] indice do NewMembers

        // Se tiver membro repetido
        if (strcmp (CurrentMembers[i].nome, NewMembers[j].nome) == 0 ) {

          // Se não forem do mesmo tamanho tem que substituir movimentando os dados
          if (CurrentMembers[i].tam_atual != NewMembers[j].tam_atual) {
            offset = CurrentMembers[i].tam_atual - NewMembers[j].tam_atual;

            // O Novo membro é maior, move os arquivos para a direita e sobreescreve
            if (offset < 0) {
              offset = -offset;  // Valor absoluto do offset
              
              // Vai lá no último membro e move a partir dele pra frente até o indice do membro exatamente igual
              for (unsigned int k = num - 1; k > i + 1; k++) {
                MoveMember (archive, CurrentMembers[k].offset, CurrentMembers[k].tam_atual, maior_buff, CurrentMembers[k].offset + offset);
                CurrentMembers[k].offset = CurrentMembers[k].offset + offset;
              }

              // Vai lá na posição e escreve o dado novo que é MAIOR
              fseek (archive, CurrentMembers[i].offset, SEEK_SET);
              NewMembers[j].offset =  CurrentMembers[i].offset;   // Atualiza o offset
              fwrite (members[j], NewMembers[j].tam_atual, 1, archive);
            }
            else {  // Move os arquivos para a esquerda
              

              // Vai no membro imediatamente depois do repetido e move ele pra esquerda, sobrescreve mesmo
              for (unsigned int k = i + 1; k < num; k++) {
                MoveMember (archive, CurrentMembers[k].offset, CurrentMembers[k].tam_atual, maior_buff, CurrentMembers[k].offset - offset);
                CurrentMembers[k].offset -= offset;
              }

              // Vai lá na pósição e escreve o dado novo que é MENOR
              fseek (archive, CurrentMembers[i].offset, SEEK_SET);
              NewMembers[j].offset =  CurrentMembers[i].offset;   // Atualiza o offset
              fwrite (members[j], NewMembers[j].tam_atual, 1, archive);
            }

            // Substitui uma struct pela outra mais atualizada e marca o UID do membro repetido
            CurrentMembers[i] = NewMembers[j];
          }
          else {  // Se forem de tamanhos iguais eu substituo direto
            // Vai lá na posição e escreve o dado novo que é do mesmo tamanho
            fseek (archive, CurrentMembers[i].offset, SEEK_SET);
            NewMembers[j].offset =  CurrentMembers[i].offset;
            fwrite (members[j], NewMembers[j].tam_atual, 1, archive);

            // Atualiza a struct
            CurrentMembers[i] = NewMembers[j];

          }

          NewMembers[j].UID = 0;   // Flag para mostrar que o arquivo já existe
          new--;
        }
      }
    }


    // Soma to número total de membros que terá o vetor raelocado
    sum = num + new;

    offset = sizeof (struct member) * new;      // Quantos bytes eu vou atualizar as structs atuais
    //Move os arquivos existentes para frente  (abre espaço para atualizar o vetor de membros)
    for (long i = (long)num-1; i >= 0; i--) {
      adress = CurrentMembers[i].offset;
      MoveMember (archive, adress, CurrentMembers[i].tam_atual, maior_buff, adress + offset);
      CurrentMembers[i].offset += offset; // Atualiza o offset imediatamente
    }


    // Fazer um realoc do CurrentMembers e inserir os membros novos no final desse buffer
    CurrentMembers = realloc (CurrentMembers, sizeof(struct member) * sum);
    if (!CurrentMembers)
      perror ("\nNão realocou o vetor de struct membros\n");


    unsigned int j = 0;  // Controla o indice do vetor CurrentMembers
    // Adiciona no final do vetor os novos membros que não se repetem
    for (unsigned int i = 0; i < new; i++) { // Controla o indice do vetor dos NewMembers
    
      if (NewMembers[i].UID != 0) {     // Se não for repetido, ele recebe no vetor e incrementa a posição, se não, ele pega o próximo
        CurrentMembers[num + j] = NewMembers[i];
        j++;
      }

    }

    fseek (archive, 0, SEEK_END);

    // Adiciona os novos arquivos ao final do archive **** NOVO
    for (unsigned int i = 0; i < new; i++) {
      NewMembers[i].offset = ftell (archive);
      fwrite (members[i], NewMembers[i].tam_atual, 1, archive);
    }


    // Move o ponteiro para a posição correta e sobreescreve o novo vetor de structs membro
    fseek (archive, 8, SEEK_SET); // Move o ponteiro para a posição que começam o membros
    fwrite (CurrentMembers, sizeof (struct member), sum, archive);
    free (CurrentMembers);
    CurrentMembers = NULL;

  } 
  else {  // Caso meu archive não tenha membro nenhum ainda
    // Ponteiro do archive está logo após do unsigned int
    fwrite (NewMembers, sizeof (struct member), new, archive);


    fseek (archive, 0, SEEK_END);

    // Adiciona os novos arquivos ao final do archive **** NOVO
    for (unsigned int i = 0; i < new; i++) {
      NewMembers[i].offset = ftell (archive); // Garante que está sendo escrito no offset certo
      fwrite (members[i], NewMembers[i].tam_atual, 1, archive);
    }

  }

  num += new;                                        // Incrementa o número de membros
  fseek (archive, 4, SEEK_SET);                      // Coloco o ponteiro no início do número de membro mais uma vez
  fwrite (&num, sizeof (unsigned int), 1, archive);   // Sobre escrevo o novo número na posição correta
  

  free (NewMembers);
  printf ("chegou no fim do FLatInsertion!!!!\n");
}


void ExtractMembers(FILE *archive, char *members[], unsigned int n_req, unsigned int n_all) {
  // 1) lê toda a tabela de structs
  struct member *tbl = malloc(sizeof *tbl * n_all);
  if (!tbl) { perror("malloc"); return; }
  fseek(archive, 8, SEEK_SET);
  if (fread(tbl, sizeof *tbl, n_all, archive) != n_all) {
    perror("fread tabela");
    free(tbl);
    return;
  }

  // 2) para cada entrada que “bate” com a lista (ou todas, se n_req==0)
  for (unsigned i = 0; i < n_all; i++) {
    if (!should_extract(tbl[i].nome, members, n_req))
      continue;

    printf("Extraindo “%s”…\n", tbl[i].nome);
    FILE *out = fopen(tbl[i].nome, "wb");
    if (!out) {
      perror(tbl[i].nome);
      continue;
    }

    // 3a) caso flat (sem compressão)
    if (tbl[i].tam_atual == tbl[i].tam_ori) {
      unsigned char *buf = malloc(tbl[i].tam_atual);
      fseek(archive, tbl[i].offset, SEEK_SET);
      fread(buf, 1, tbl[i].tam_atual, archive);
      fwrite(buf, 1, tbl[i].tam_atual, out);
      free(buf);

    // 3b) caso comprimido (tam_atual < tam_ori)
    } else {
      // pula os 2 cabeçalhos [csz][ori] no arquivo
      long data_off = tbl[i].offset + 2 * sizeof(unsigned int);
      unsigned int csz = tbl[i].tam_atual;
      unsigned int ori = tbl[i].tam_ori;

      unsigned char *compr = malloc(csz);
      fseek(archive, data_off, SEEK_SET);
      fread(compr, 1, csz, archive);

      unsigned char *flat = malloc(ori);
      LZ_Uncompress(compr, flat, csz);

      fwrite(flat, 1, ori, out);
      free(compr);
      free(flat);
    }

    fclose(out);
  }

  free(tbl);
}