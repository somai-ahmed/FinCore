#include <Src/FinCore.Native/Src/Utils/include/comptes.h>

#define COMPTES_CODE_LONGUEUR_MIN 3
#define COMPTES_CODE_LONGUEUR_MAX 15

bool comptes_valider_code(const char *code){

    if (code == NULL) {
        return false;
    }
    return valider_code_compte(code, COMPTES_CODE_LONGUEUR_MIN, COMPTES_CODE_LONGUEUR_MAX); /*validation.h*/
}

Etat comptes_creer(Compte *compte, const char *code, const char *nom, ClasseCompte classe, TypeCompte type, SoldeNormal solde_normal, id_compte parent_id)

/* la fonction va retourner l'etat du l'instruction */
/* etat est un type d'une structure enum deja implemente en ERROR.H */
    if (compte == NULL || code == NULL || nom == NULL) {
        return ERR_POINTEUR_NULLE;
    }

    if (!comptes_valider_code(code)) {
        return ERR_CODE_COMPTE_INVALIDE;
    }

    if (classe < CLASSE_1_CP_ET_PNC || classe > CLASSE_7_PRODUITS) {
        return ERR_CLASSE_COMPTE_INVALIDE;
    }

    if (type < COMPTE_ACTIF || type > COMPTE_CHARGE) {
        return ERR_TYPE_COMPTE_INVALIDE;
    }

    if (solde_normal != SOLDE_DEBITEUR && solde_normal != SOLDE_CREDITEUR) {
        return ERR_ARGUMENT_INVALIDE;
    }

    if (nom[0] == '\0') {
        return ERR_ARGUMENT_INVALIDE;
    }

    /* convention PCG : le premier chiffre du code doit correspondre a la classe */
    if ((code[0] - '0') != (int)classe) {
        return ERR_CLASSE_COMPTE_INVALIDE;
    }

    /* le moteur a depasse tous les erreurs donc il va creer le compte correctement */
    compte->id = INVALID_ID; 

    strncpy(compte->code, code, sizeof(compte->code) - 1);
    compte->code[sizeof(compte->code) - 1] = '\0';

    strncpy(compte->nom, nom, sizeof(compte->nom) - 1);
    compte->nom[sizeof(compte->nom) - 1] = '\0';

    compte->classe       = classe;
    compte->type         = type;
    compte->solde_normal = solde_normal;
    compte->solde        = 0;
    compte->parent_id    = parent_id;
    compte->est_active   = 1;

    return ETAT_OK; /* success */
/* en code dans main , si la fonction ne retorne pas l'etat ok , il y a un erreur et l'instruction est elimine*/
}
