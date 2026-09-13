#include "string.h"
#include <ctype.h>

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

bool chaine_est_numerique(const char *chaine) {
    if (chaine == NULL || !chaine_non_vide(chaine)) return false;

    for (const char *p = chaine; *p != '\0'; p++) {
        if (!isdigit((unsigned char)*p))
            return false;
    }
    return true;
}
  
