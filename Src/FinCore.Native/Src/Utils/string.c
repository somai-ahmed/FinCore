#include "string.h"

int longeurchaine(const char *chaine){
  int len = 0;
  if (chaine) {
    for (; chaine[len] != '\0'; len++) {
    }
  }
  return len ;
}

bool chaine_non_vide(const char *chaine){ 
  return (chaine != NULL && longeurchaine(chaine) > 0);
}

bool chaine_est_numerique(const char *chaine){ /*is digit function in the real standard C99 library "string.h" */
  
