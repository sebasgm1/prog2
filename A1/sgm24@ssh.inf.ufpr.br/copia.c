#include "./vinac.h"




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

  free (buffer);
  buffer = NULL;

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
  unsigned int sum, adress;   // Soma do número de membros novos com número dos anteriores; Distância que vou mover os arquivos
  
  // FILE *f;                   // Ponteiro auxiliar para guardar o arquivo
  long tam = 0;                  // tamanho do arquivo inserido
  long int maior_buff = 0, offset = 0;                  // tamanho do arquivo inserido


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



void InsertCompressed (FILE *archive, char **members, unsigned int n_new) {
  unsigned int n_old, bigger_new = 0;
  struct member *old_tbl = NULL, *new_tbl = NULL;
  // Cursor: usado para calcular o endereço de escrtia do new; shift_struct: guarda o tamanho do shift para a direita que os arquivos tem que dar
  long cursor, shift_struct;
  long int maior_buff = 0;
  FILE *new;

  if (n_new < 1)  {
    perror ("Quer inserir comprimido < 1 membros\n");
    return;
  }

  unsigned char *files [n_new];
  
  // Lé o número de membros atualmente
  fseek (archive, 4, SEEK_SET);
  if (fread (&n_old, sizeof (unsigned int), 1, archive) != 1) {    // Leio o número de membros atualmente
    perror ("não conseguiu ler\n");
    return;
  }

  // Se tiver membros, testa pra ver se existem repetidos
  if (n_old) {
    old_tbl = malloc (sizeof (struct member) * n_old);
    fseek (archive, 8, SEEK_SET);
    fread (old_tbl, sizeof (struct member), n_old, archive);

    // Procuro qual seria o membro com o maior tamanho pra fazer o buffer (vou usar no MoveMember)
    for (unsigned int i = 0; i < n_old; i++) {      // Procura o maior tamanho dentro da old_tbl
      if (old_tbl[i].tam_atual > maior_buff)
        maior_buff = old_tbl[i].tam_atual;
    }

    // Vou conferindo se tem membros repetidos
    for (unsigned int i = 0; i < n_old; i++) {
      if (old_tbl[i].UID == 0) {
      for (unsigned int j = 0; j < n_new; j++) {    // Indice dos membros novos
        if (strcmp (old_tbl[i].nome, members[j]) == 0)
          old_tbl[i].UID = 0;         // Flag que marca que ele é repetido
          old_tbl[i].tam_ori = j;     // Guarda o indice do membro repetido
      }
    }


  }

  // Procuro qual seria o membro com o maior tamanho pra fazer o buffer (vou usar no MoveMember)
  for (unsigned int i = 0; i < n_old; i++) {      // Procura o maior tamanho dentro da old_tbl
    if (old_tbl[i].tam_atual > maior_buff)
      maior_buff = old_tbl[i].tam_atual;
  }
  
  

  fseek (archive, 0, SEEK_END);
  cursor = ftell (archive);
  cursor += sizeof (struct member) * n_new;       // Determina aonde que os novos membros vão ser escritos
  
  // Nesse for vão ser carregados todos os membros a partir do vetor de chars, members
  new_tbl = malloc (sizeof (struct member) * n_new);
  for (unsigned int i = 0; i<n_new; i++) {

    // Abre o membro
    new = fopen (members [i], "rb");
    if (!new) {
      perror (members[i]);
      return;
    }

    // Pega o tamanho dele plano
    fseek(new, 0, SEEK_END);
    long insize = ftell(new);
    rewind(new);

    unsigned char *flat = malloc(insize);          // Aloca memória para guardar ele plano
    fread(flat, 1, insize, new);                  // Transcreve o arquivo que estava no disco para a memória (ainda está plano)
    fclose(new);

    unsigned char *compressed = malloc (insize * 2);
    unsigned int csz = LZ_Compress (flat, compressed, insize);    // Comprime aqui!
    if (csz > insize || csz <= 0) {
      csz = insize;
      memcpy(compressed, flat, insize);            //  Copia ao conteúdo do flat pro compr e tem insize tamanhos
    }
    free (flat);

    // Preenche a struct
    strncpy(new_tbl[i].nome, members[i], sizeof new_tbl[i].nome-1);     // Copia o nome do membro pra dentro da struct
    new_tbl[i].nome[sizeof new_tbl[i].nome-1] = '\0';                   // Coloca \0 no final do nome

    new_tbl[i].UID = getuid ();
    new_tbl[i].tam_ori = insize;
    new_tbl[i].tam_atual = csz;
    new_tbl[i].data = time(NULL);
    new_tbl[i].offset = cursor;

    // Guardo o ponteiro do arquivo comprimido em outro vetor
    files[i] = compressed;

    free (compressed);
    compressed = NULL;
    // Calcula o novo endereço do próximo arquivo a ser incluido
    cursor += csz;

    if (csz > bigger_new) // Guarda o maior tamanho de arquivo comprimido
      bigger_new = csz;

  }

  for (unsigned int i = 0; i < n_old; i++) {      // Indice da tabela antiga
    if (old_tbl[i].UID == 0) {
      
    }
  }





  // Bytes aonde cabe os outros membros
  shift_struct = sizeof (struct member) * n_new;

  if (bigger_new > maior_buff)    // Compara qual vai ser o maior tamanho
    maior_buff = bigger_new;

  // For que vai mover todos os membros, abrindo espaço para adicionar as novas structs dos novos arquivos
  for (int i = n_old - 1; i >= 0; i--) {
    MoveMember (archive, old_tbl[i].offset, old_tbl[i].tam_atual, maior_buff, old_tbl[i].offset + shift_struct);
    old_tbl[i].offset += shift_struct;
  }

  // Posiciona o ponteiro exatamente aonde quero adicionar os novos memebros
  fseek (archive, 8, SEEK_SET);
  fseek (archive, n_old * sizeof (struct member), SEEK_CUR);
  fwrite (new_tbl, sizeof *new_tbl, n_new, archive);        // Escrevo a nova tabela


  // Gravar os blocos comprimidos
  for (unsigned int i = 0; i < n_new; i++) {
    unsigned char *compressed = files[i];
    unsigned int csz = new_tbl[i].tam_atual;
    long offset = new_tbl[i].offset;

    fseek (archive, offset, SEEK_SET);
    fwrite (compressed, csz, 1, archive);
  }

  // Escreve o novo número de membros no diretório
  fseek (archive, 4, SEEK_SET);

  unsigned int sum = n_old + n_new;
  fwrite (&sum, sizeof n_new, 1, archive);
  fflush (archive);

  free (old_tbl);
  free (new_tbl);

}




// members_to_remove: vetor de strings com nomes a remover
// n_remove: quantos nomes no vetor
void RemoveMembers (FILE *archive, char **targets, unsigned int n) {
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
  for (unsigned int i = 0; i < ori_sz; i++) {      // Procura o maior tamanho dentro da old_tbl
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

  // Aqui nestre for eu vou preencher a nova tabela
  for (unsigned int i = 0; i < ori_sz; i++) {

    if (old_tbl[i].UID != 0) {    // Essa marcação no UID foi feita no for de cima aonde eu removi os aquivos
      new_tbl[j] = old_tbl[i];
      j++;
    }
  }


  maior_buff = 0;
  // Procuro qual seria o membro com o maior tamanho pra fazer o buffer (vou usar no MoveMember)
  for (unsigned int i = 0; i < new_sz; i++) {      // Procura o maior tamanho dentro da new_tbl
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
  free (old_tbl);
  old_tbl = NULL;
  free (new_tbl);
  new_tbl = NULL;
  
  // Escreve o novo número de membros presentes no d  iretório
  fseek (archive, 4, SEEK_SET);
  fwrite (&new_sz, sizeof (unsigned int), 1, archive);

  rewind (archive); 
}




// Retorna verdadeiro se devemos extrair esta entrada
static int should_extract(const char *name, char *list[], unsigned cnt) {
    if (cnt == 0) return 1;              // se nenhum nome passado, extrai tudo
    for (unsigned i = 0; i < cnt; i++)
        if (strcmp(name, list[i]) == 0) return 1;
    return 0;
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