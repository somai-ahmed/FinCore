#include <Src/FinCore.Native/Src/Utils/include/identifiants.h>
#include <string.h>

generateur_id initiation_generateur_id(int64_t valeur_depart){
    generateur_id gen;
    gen.compteur = valeur_depart;
    return gen;
}
