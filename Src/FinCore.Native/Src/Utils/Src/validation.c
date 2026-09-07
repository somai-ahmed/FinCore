#include <Src/FinCore.Native/Src/Utils/include/validation.h>

#include <string.h>

bool valider_chaine_non_vide(const char *chaine){
  if(chaine==NULL) return false;
  if(*chaine == '\0') return false;  
  return true;
}
