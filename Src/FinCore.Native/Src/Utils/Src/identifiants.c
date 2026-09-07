#include <Src/FinCore.Native/Src/Utils/include/identifiants.h>
#include <string.h>

generateur_id initiation_generateur_id(int64_t valeur_depart){
    generateur_id gen;
    gen.compteur = valeur_depart;
    return gen;
}

int64_t generateur_id_suivant(generateur_id *gen);
    if (!gen) return -1;
    gen->compteur++;
    return gen->compteur;
}

bool generateur_id_reference(generateur_id *gen, const char *prefixe, int largeur_min, char *tampon, size_t taille_tampon){
    if (!gen || !prefixe || largeur_min ==0 ||taille_tampon <0) return false ;
    
