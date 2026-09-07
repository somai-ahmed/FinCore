#include <Src/FinCore.Native/Src/Utils/include/validation.h>

#include <string.h>

bool valider_chaine_non_vide(const char *chaine){
  if(chaine==NULL) return false;
  if(*chaine == '\0') return false;  
  return true;
}

bool valider_longueur_chaine(const char *chaine, size_t min, size_t max){
    if(chaine==NULL) return false;
    if(!min ||!max || min=max) return false;

    size_t longueur = strlen(chaine);

    return longueur >= min && longueur <= max;
}
