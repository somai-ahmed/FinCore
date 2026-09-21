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



static void bilan_ajouter_ligne(ligne_bilan *sortie, size_t capacite, size_t *nb_lignes, const char *libelle, Monnaie montant, int est_sous_total, int profondeur){
 
    if (*nb_lignes < capacite) {
        memset(&sortie[*nb_lignes], 0, sizeof(sortie[*nb_lignes]));
        snprintf(sortie[*nb_lignes].libelle, sizeof(sortie[*nb_lignes].libelle), "%s", libelle);
        sortie[*nb_lignes].montant = montant;
        sortie[*nb_lignes].est_sous_total = est_sous_total;
        sortie[*nb_lignes].profondeur = profondeur;
    }
 
    (*nb_lignes)++;
}
 
/* ---------------------------------------------------
                  LES SOMMES PAR CLASSE
---------------------------------------------------- */
 
/* la somme des soldes d une classe de comptes (les classes vont de 1 a 7)
   - sens = SOLDE_DEBITEUR : on additionne les solde_debit des comptes de la classe
   - sens = SOLDE_CREDITEUR : on additionne les solde_credit des comptes de la classe
   il faut les comptes car une ligne de balance ne contient pas la classe : on la retrouve avec l id du compte
   si un pointeur est NULL la fonction retourne 0 ; une ligne de balance dont le compte est inconnu est ignoree
   (bilan_generer controle ca AVANT d appeler cette fonction)
   voir documentation/comptabilite/finctionnement_du_bilan.ipynb
   */
Monnaie bilan_somme_soldes(const Compte *comptes, size_t nb_comptes, const LigneBalance *balance, size_t nb_balance, ClasseCompte classe, SoldeNormal sens){
    Monnaie somme = 0;
    size_t i, indice;
 
    if (!comptes || !balance) return 0;
 
    for (i = 0; i < nb_balance; i++) {
 
        /* on cherche le compte de cette ligne de balance pour connaitre sa classe */
        if (trouver_indice_compte(comptes, nb_comptes, balance[i].compte.id, &indice) != ETAT_OK)
            continue;
 
        if (comptes[indice].classe != classe)
            continue;
 
        if (sens == SOLDE_DEBITEUR)
            somme += balance[i].solde_debit;
        else
            somme += balance[i].solde_credit;
    }
 
    return somme;
}

 
/* le solde NET d une classe = (somme des soldes debiteurs) - (somme des soldes crediteurs)
   positif = la classe est globalement debitrice, negatif = globalement creditrice
   c est ce qui permet aux comptes d amortissement (classe 2, solde crediteur) de se retirer de leur classe */
static Monnaie net_classe(const Compte *comptes, size_t nb_comptes, const LigneBalance *balance, size_t nb_balance, ClasseCompte classe){
 
    return bilan_somme_soldes(comptes, nb_comptes, balance, nb_balance, classe, SOLDE_DEBITEUR)
         - bilan_somme_soldes(comptes, nb_comptes, balance, nb_balance, classe, SOLDE_CREDITEUR);
}
 
/* le resultat de l exercice = les produits nets (classe 7) - les charges nettes (classe 6)
   positif => benefice, negatif => perte
   si des ecritures de cloture ont deja vide les classes 6 et 7, le resultat ici est 0 (il est deja dans la classe 1) : pas de double comptage */
Monnaie bilan_resultat(const Compte *comptes, size_t nb_comptes, const LigneBalance *balance, size_t nb_balance){
    Monnaie produits_nets;
    Monnaie charges_nettes;
 
    if (!comptes || !balance) return 0;
   
    /* pour les produits nets : credit(solde_crediteur) - debit(solde_debiteur)
       pour les charges nettes : debit(solde_debiteur) - credit(solde_crediteur) */
   
    produits_nets  = bilan_somme_soldes(comptes, nb_comptes, balance, nb_balance, CLASSE_7_PRODUITS, SOLDE_CREDITEUR)
                   - bilan_somme_soldes(comptes, nb_comptes, balance, nb_balance, CLASSE_7_PRODUITS, SOLDE_DEBITEUR);
   
    charges_nettes = bilan_somme_soldes(comptes, nb_comptes, balance, nb_balance, CLASSE_6_CHARGES, SOLDE_DEBITEUR)
                   - bilan_somme_soldes(comptes, nb_comptes, balance, nb_balance, CLASSE_6_CHARGES, SOLDE_CREDITEUR);
 
    return produits_nets - charges_nettes;
}
 
