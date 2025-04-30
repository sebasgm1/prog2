#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "vinac.h"


void comprime (unsigned char *in, unsigned char *out, unsigned int insize) {
  if(!in || !out) {
    printf ("COMPRIME: Parametros NULL\n");
    return;
  }

  // Um int com o tamanho dos dados comprimidos
  int comprimido = LZ_Compress(in, out, insize );

  if (comprimido >= insize) {
    // então você deve armazenar os dados descomprimidos.
  }



}