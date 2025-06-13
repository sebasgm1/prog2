#include <stdio.h>

#include "Joystick.h"


struct joystick *create_joystick () {
  struct joystick *j = malloc (sizeof (struct joystick));
  if (!j) {
    perror ("Erro no malloc do create_joystick\n");
    exit (0);
  }

  // Inicializa os botões do joystick
  j->right = 0;
  j->left = 0;
  j->up = 0;
  j->down = 0;
  j->fire = 0;
  j->run = 0;

  return j;
}

void destroy_joystick (struct joystick *j) {
  if (!j) {
    perror ("Erro no destroy_joystick: ponteiro nulo\n");
    return;
  }
  free (j);
  j = NULL;
}