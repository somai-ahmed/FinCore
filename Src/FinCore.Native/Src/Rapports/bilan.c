#include <stdio.h>
#include <string.h>
#include "src/FinCore.Native/include/rapports.h"

/* le bilan : une photo de l entreprise a une date d arret (ce qu elle possede = ACTIF, comment c est finance = PASSIF)
   ce n est pas un flux sur une periode comme la balance de janvier : le bilan a besoin des soldes CUMULES depuis le debut
   donc la balance qu on lui donne doit etre generee avec un debut tres ancien (ex. 1900-01-01) et fin = date d arret :

    le bilan ne lit pas les ecritures : il se construit seulement avec la balance et les comptes (pour connaitre la classe de chaque compte)
 
   les fonctions "STATIC" sont propres a ce fichier : ce sont les petits outils du bilan
   (trouver_indice_compte est la meme que dans balance.c et grand_livre.c) */
 
/* le bilan a toujours la meme forme : 15 lignes (les titres et les deux totaux) */
#define NB_LIGNES_BILAN 15

/* Indice du compte dans comptes[] */
static Etat trouver_indice_compte(const Compte *comptes, size_t nb_comptes, id_compte id, size_t *indice_out){
    size_t i;
 
    for (i = 0; i < nb_comptes; i++) {
        if (comptes[i].id == id) {
            *indice_out = i;
            return ETAT_OK;
        }
    }
    return ERR_COMPTE_INTROUVABLE;
}

















Monnaie bilan_somme_soldes(const Compte *comptes, size_t nb_comptes,
                           const LigneBalance *balance, size_t nb_balance,
                           ClasseCompte classe, SoldeNormal sens);
Monnaie bilan_resultat(const Compte *comptes, size_t nb_comptes,
                       const LigneBalance *balance, size_t nb_balance);
Etat    bilan_verifier(Monnaie total_actif, Monnaie total_passif);
