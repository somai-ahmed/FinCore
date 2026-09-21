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


/* -------------------------------------------------------------------------
   LE SOLDE INITIAL : ce que valait le compte avant la plage demandee (SI)
   ----------------------------------------------------------------------- */
 
/* le solde initial d un compte = son solde AVANT la date avant_date (le report des periodes precedentes)
   on additionne l effet de toutes les lignes du compte, dans les ecritures comptabilisees datees strictement avant avant_date
   le resultat est dans le sens normal du compte (voir effet_sur_solde)
   si un pointeur est NULL la fonction retourne 0 (sous le type Monnaie) */
Monnaie grand_livre_solde_initial(const Compte *compte, const Ecriture *ecritures, size_t nb_ecritures, DATE avant_date){
    Monnaie solde = 0;
    size_t j, k;
 
    if (!compte || !ecritures) return 0;
 
    for (j = 0; j < nb_ecritures; j++) {
        const Ecriture *ecr = &ecritures[j];
 
         /* Seules les ecritures comptabilisees anterieures a avant_date
            sont prises en compte dans le report */
         if (!ecritures_est_comptabilisee(ecr) || comparer_dates(ecr->date, avant_date) >= 0)
             continue;
         
         if (!ecr->lignes)
             continue;
 
        for (k = 0; k < ecr->nombre_lignes; k++) {
            const ligne_journal *ligne = &ecr->lignes[k];
            Monnaie d = ligne->Debit;
            Monnaie c = ligne->credit;
 
            if (ligne->compte_id == compte->id)
                solde += effet_sur_solde(compte, d, c);
        }
    }
 
    return solde;
}

/* -------------------------------------------------------------
                  LA GENERATION DU GRAND LIVRE 
   ------------------------------------------------------------- */

Etat grand_livre_generer(id_compte compte_id, const Compte *comptes, size_t nb_comptes, const Ecriture *ecritures, size_t nb_ecritures, DATE debut, DATE fin, Entree_GrandLivre *sortie, size_t capacite, size_t *nb_sortie){
 
    if (!comptes || !nb_sortie) return ERR_POINTEUR_NULLE;
    if (!ecritures && nb_ecritures > 0) return ERR_POINTEUR_NULLE;
    if (!periodes_plage_valide(debut, fin)) return ERR_PLAGE_DATES_INVALIDE;
    if (compte_id == INVALID_ID) return ERR_IDENTIFIANT_INVALIDE;

    size_t i, j, k, indice;
    size_t nb_entrees = 0;
    const Compte *compte;
    Monnaie cumul;
    Etat etat;
   
    /* retrouver le compte avec l'appelle du la fonction trouver_indice_compte */
    etat = trouver_indice_compte(comptes, nb_comptes, compte_id, &indice);
    if (etat != ETAT_OK)
        return etat;
    compte = &comptes[indice];
 
    /*on COMPTE seulement les mouvements du compte, sans rien ecrire
          ca permet d annoncer la taille requise (appel avec capacite = 0) */
    for (j = 0; j < nb_ecritures; j++) {
        const Ecriture *ecr = &ecritures[j];
 
        if (!ecriture_retenue(ecr, debut, fin))
            continue;
        if (ecr->nombre_lignes > 0 && ecr->lignes == NULL)
            return ERR_POINTEUR_NULLE;
 
        for (k = 0; k < ecr->nombre_lignes; k++) {
            if (ecr->lignes[k].compte_id == compte_id)
                nb_entrees++;
        }
    }
 
    *nb_sortie = nb_entrees;
    if (capacite < nb_entrees)
        return ERR_TRES_PETIT_BUFFER;
    if (nb_entrees == 0)
        return ERR_AUCUNE_DONNEE_PERIODE;
    if (sortie == NULL)
        return ERR_POINTEUR_NULLE;
 
    /* second passage : on REMPLIT sortie, une entree par ligne d ecriture qui touche le compte
          (une ecriture qui a 2 lignes sur ce compte donne donc 2 entrees)
          la fonction snprintf est la meme que dans balance.c : elle copie la chaine vers la destination sans depasser sa taille du la biblio <string.h>
          <voir documentation/string_func/snprintf.ipynb>
          */
   
    i = 0;
    for (j = 0; j < nb_ecritures; j++) {
        const Ecriture *ecr = &ecritures[j];
 
        if (!ecriture_retenue(ecr, debut, fin))
            continue;
 
        for (k = 0; k < ecr->nombre_lignes; k++) {
            const ligne_journal *ligne = &ecr->lignes[k];
            Monnaie d = ligne->Debit;
            Monnaie c = ligne->credit;
 
            if (ligne->compte_id != compte_id)
                continue;
 
            memset(&sortie[i], 0, sizeof(sortie[i]));
            sortie[i].date = ecr->date;
            snprintf(sortie[i].reference, sizeof(sortie[i].reference), "%s", ecr->reference);
 
            /* le libelle du mouvement : celui de la ligne, ou celui de l ecriture si la ligne n en a pas */
            if (ligne->libelle[0] != '\0')
                snprintf(sortie[i].libelle, sizeof(sortie[i].libelle), "%s", ligne->libelle);
            else
                snprintf(sortie[i].libelle, sizeof(sortie[i].libelle), "%s", ecr->description);
 
            sortie[i].debit  = d;
            sortie[i].credit = c;
            i++;
        }
    }
 
      /* Trier les mouvements avant de calculer le solde cumule */
      qsort(sortie, nb_entrees, sizeof(sortie[0]), comparer_entrees_grand_livre);
      
      /* Partir du solde initial et appliquer chaque mouvement chronologiquement */
      cumul = grand_livre_solde_initial(compte, ecritures, nb_ecritures, debut);
      
      for (i = 0; i < nb_entrees; i++) {
          cumul += effet_sur_solde(compte, sortie[i].debit, sortie[i].credit);
          sortie[i].solde_cumule = cumul;
      }
   
      return ETAT_OK;
}

/* ------------------------------------
      LA VERIFICATION DU GRAND LIVRE
  ------------------------------------*/
 
/* la verification du grand livre d un compte :
   ligne est la ligne de la balance de CE compte, calculee sur la meme plage de dates que le grand livre
   le total des debits et des credits des entrees doit etre egal a total_debit et total_credit de cette ligne
   elle retourne le premier probleme trouve sous forme de code d erreur */

Etat grand_livre_verifier(const Entree_GrandLivre *entrees, size_t nombre, const LigneBalance *ligne){
    Monnaie somme_debit = 0;
    Monnaie somme_credit = 0;
    size_t i;
 
    if (!ligne) return ERR_POINTEUR_NULLE;
    if (!entrees && nombre > 0) return ERR_POINTEUR_NULLE;
 
    for (i = 0; i < nombre; i++) {
 
        /* un montant negatif n a pas de sens dans un mouvement du grand livre */
        if (entrees[i].debit < 0 || entrees[i].credit < 0)
            return ERR_MOUVEMENT_COMPTABLE_INVALIDE;
 
        somme_debit  += entrees[i].debit;
        somme_credit += entrees[i].credit;
    }
 
    /* si le grand livre est vide alors que la balance a des mouvements, les sommes sont differentes : le grand livre est incoherent */
    if (somme_debit != ligne->total_debit || somme_credit != ligne->total_credit)
        return ERR_GRAND_LIVRE_BALANCE_INCOHERENTS;
 
    return ETAT_OK;
}
