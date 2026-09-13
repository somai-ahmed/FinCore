#include "string.h"
#include <ctype.h>
#include <stdeff.h>
#include <stdbool.h>

size_t longeurchaine(const char *chaine){
  size_t len = 0;
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
  
bool ch_copy_securise(char *dest, const char *src, size_t taille_dest){
    if (dest == NULL || src == NULL || taille_dest == 0)
        return false;

    size_t i ;

    while (i < taille_dest - 1 && src[i] != '\0') {
        dest[i] = src[i];
        i++;
    }

    dest[i] = '\0';
    return true;
}

void chaine_rogner(char *chaine)
