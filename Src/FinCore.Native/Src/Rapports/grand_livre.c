#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "src/FinCore.Native/include/rapports.h"
#include "src/FinCore.Native/include/ecritures.h"
#include "src/FinCore.Native/include/periodes.h"

/* le grand livre : pour UN SEUL compte, la liste de tous ses mouvements (une entree par ligne d ecriture)
   classes par date, avec le solde cumule apres chaque mouvement
   la generation du grand livre general (tous les comptes) se fait en appelant grand_livre_generer compte par compte
 
   les fonctions "STATIC" sont propres a ce fichier : ce sont les petits outils du grand livre */


/* l'explication du tous ce fonctions conecernat la comptabilite :: voir documentation/comptabilite/grand_ivre.ipynb*/

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
 
/* Une écriture compte si elle est comptabilisée et datée dans [debut, fin] */
static bool ecriture_retenue(const Ecriture *ecr, DATE debut, DATE fin){
 
    return ecritures_est_comptabilisee(ecr) && comparer_dates(ecr->date, debut) >= 0 && comparer_dates(ecr->date, fin) <= 0;
}

/* l effet d une ligne sur le solde du compte, dans le sens NORMAL du compte :
   - compte debiteur (actif, charges) : le solde augmente avec le debit --> debit - credit
   - compte crediteur (passif, produits) : le solde augmente avec le credit --> credit - debit
   un resultat negatif veut dire que le solde est a l envers du sens normal du compte */
static Monnaie effet_sur_solde(const Compte *compte, Monnaie debit, Monnaie credit){
 
    if (compte->solde_normal == SOLDE_CREDITEUR)
        return credit - debit;
 
    return debit - credit;
}

/* la fonction de comparaison donnee a qsort : elle dit lequel de deux mouvements passe avant l autre
   - qsort recoit des adresses "void *" (adresse sans type) : on les remet au type Entree_GrandLivre avant de s en servir
   - on compare d abord les dates avec comparer_dates, et si la date est la meme on departage avec la reference (strcmp compare deux chaines de caracteres)
   - le resultat suit la meme regle que comparer_dates : negatif = a passe avant b, 0 = egaux, positif = a passe apres b */
static int comparer_entrees_grand_livre(const void *a, const void *b){
    const Entree_GrandLivre *entree_a = (const Entree_GrandLivre *)a;
    const Entree_GrandLivre *entree_b = (const Entree_GrandLivre *)b;
    int resultat = comparer_dates(entree_a->date, entree_b->date);
 
    if (resultat != 0)
        return resultat;
 
    return strcmp(entree_a->reference, entree_b->reference);
}
