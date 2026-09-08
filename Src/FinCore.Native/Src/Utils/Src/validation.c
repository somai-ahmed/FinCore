#include <Src/FinCore.Native/Src/Utils/include/validation.h>

#include <string.h>
#include <ctype.h>

bool valider_chaine_non_vide(const char *chaine){
  if(chaine==NULL) return false;
  if(*chaine == '\0') return false;  
  return true;
}

bool valider_longueur_chaine(const char *chaine, size_t min, size_t max){
    if(chaine==NULL) return false;
    if(min > max) return false;

    size_t longueur = strlen(chaine);

    return longueur >= min && longueur <= max;
}


bool valider_caracteres_autorises(const char *chaine, const char *caracteres_extra){
    if (chaine == NULL) return false;

    const char *p = chaine;
    while (*p != '\0') {
        if (isalnum((unsigned char)*p)) {
            p++;
            continue;
        }
        if (*p == ' ') {
            p++;
            continue;
        }
        if (caracteres_extra != NULL && strchr(caracteres_extra, *p) != NULL) {
            p++;
            continue;
        }
        return false;
    }
    return true;
}



