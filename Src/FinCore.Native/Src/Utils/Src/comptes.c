#include <Src/FinCore.Native/Src/Utils/include/comptes.h>

#define COMPTES_CODE_LONGUEUR_MIN 3
#define COMPTES_CODE_LONGUEUR_MAX 15

bool comptes_valider_code(const char *code){

    if (code == NULL) {
        return false;
    }
    return valider_code_compte(code, COMPTES_CODE_LONGUEUR_MIN, COMPTES_CODE_LONGUEUR_MAX); /*validation.h*/
}
