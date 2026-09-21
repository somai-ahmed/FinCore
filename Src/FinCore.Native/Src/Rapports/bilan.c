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
 
/*-----------------------------------------------------------------
                  LA GENERATION DU BILAN 
-----------------------------------------------------------------*/
 
Etat bilan_generer(const Compte *comptes, size_t nb_comptes, const LigneBalance *balance, size_t nb_balance, ligne_bilan *sortie, size_t capacite, size_t *nb_sortie, Monnaie *total_actif, Monnaie *total_passif){
 
    size_t i, indice;
    size_t nb_lignes = 0;
    Etat etat;
    Monnaie actifs_non_courants, stocks, tiers_debiteurs, tresorerie, actifs_courants;
    Monnaie capitaux_classe_1, resultat, fonds_propres, tiers_crediteurs, concours_bancaires, passifs_courants;
 
 
    if (!comptes || !balance || !nb_sortie || !total_actif || !total_passif) return ERR_POINTEUR_NULLE;

    /*-------------------------initialisation-----------------------------*/
    /* les totaux repartent a zero et on annonce toujours la taille requise : 15 lignes */
    *total_actif = 0;
    *total_passif = 0;
    *nb_sortie = NB_LIGNES_BILAN;
   
    if (capacite < NB_LIGNES_BILAN)
        return ERR_TRES_PETIT_BUFFER;
    if (sortie == NULL)
        return ERR_POINTEUR_NULLE;
 
    /* sans aucune ligne de balance il n y a rien a mettre dans le bilan */
    if (nb_balance == 0) return ERR_DONNEES_BILAN_INSUFFISANTES;
 
    /* 1. le controle de la balance : chaque ligne doit avoir un compte connu et une classe valide (de 1 a 7)
          sinon son solde ne serait dans aucun poste et le bilan serait faux sans qu on sache pourquoi */
    for (i = 0; i < nb_balance; i++) {
        etat = trouver_indice_compte(comptes, nb_comptes, balance[i].compte.id, &indice);
        if (etat != ETAT_OK)
            return etat;
        if (comptes[indice].classe < CLASSE_1_CP_ET_PNC || comptes[indice].classe > CLASSE_7_PRODUITS)
            return ERR_CLASSE_COMPTE_INVALIDE;
    }
 
    /* 2. le calcul des postes de l ACTIF (ce que l entreprise possede)
        - classes 2 et 3 : en NET (les amortissements et les provisions se retirent de leur classe)
        - classes 4 et 5 : compte par compte selon le signe, seuls les soldes DEBITEURS sont a l actif */
    actifs_non_courants = net_classe(comptes, nb_comptes, balance, nb_balance, CLASSE_2_ACTIFS_NON_COURANTS);
    stocks              = net_classe(comptes, nb_comptes, balance, nb_balance, CLASSE_3_STOCKS);
    tiers_debiteurs     = bilan_somme_soldes(comptes, nb_comptes, balance, nb_balance, CLASSE_4_TIERS, SOLDE_DEBITEUR);
    tresorerie          = bilan_somme_soldes(comptes, nb_comptes, balance, nb_balance, CLASSE_5_TRESORERIE, SOLDE_DEBITEUR);
    actifs_courants     = stocks + tiers_debiteurs + tresorerie;
 
    /* 3. le calcul des postes du PASSIF (comment c est finance)
        - classe 1 : le net est negatif (soldes crediteurs) donc on le remet en positif avec le signe -
        - le resultat de l exercice s ajoute aux capitaux propres
        - classes 4 et 5 : seuls les soldes CREDITEURS sont au passif (un client crediteur ou une banque a decouvert par exemple) */
    capitaux_classe_1   = -net_classe(comptes, nb_comptes, balance, nb_balance, CLASSE_1_CP_ET_PNC);
    resultat            = bilan_resultat(comptes, nb_comptes, balance, nb_balance);
    fonds_propres       = capitaux_classe_1 + resultat;
    tiers_crediteurs    = bilan_somme_soldes(comptes, nb_comptes, balance, nb_balance, CLASSE_4_TIERS, SOLDE_CREDITEUR);
    concours_bancaires  = bilan_somme_soldes(comptes, nb_comptes, balance, nb_balance, CLASSE_5_TRESORERIE, SOLDE_CREDITEUR);
    passifs_courants    = tiers_crediteurs + concours_bancaires;
 
    *total_actif  = actifs_non_courants + actifs_courants;
    *total_passif = fonds_propres + passifs_courants;
 
    /* les lignes du bilan dans l ordre d affichage : bilan_ajouter_ligne(sortie, capacite, &nb_lignes, libelle, montant, est_sous_total, profondeur)
          les libelles sont sans accents pour eviter les problemes d encodage entre le compilateur et l interface
          les lignes est_sous_total = 1 sont des totaux : il ne faut pas les additionner avec les postes qu elles contiennent */
    bilan_ajouter_ligne(sortie, capacite, &nb_lignes, "ACTIF", 0, 0, 0);
    bilan_ajouter_ligne(sortie, capacite, &nb_lignes, "Actifs non courants", actifs_non_courants, 0, 1);
    bilan_ajouter_ligne(sortie, capacite, &nb_lignes, "Actifs courants", actifs_courants, 1, 1);
    bilan_ajouter_ligne(sortie, capacite, &nb_lignes, "Stocks", stocks, 0, 2);
    bilan_ajouter_ligne(sortie, capacite, &nb_lignes, "Tiers debiteurs", tiers_debiteurs, 0, 2);
    bilan_ajouter_ligne(sortie, capacite, &nb_lignes, "Tresorerie", tresorerie, 0, 2);
    bilan_ajouter_ligne(sortie, capacite, &nb_lignes, "TOTAL ACTIF", *total_actif, 1, 0);
 
    bilan_ajouter_ligne(sortie, capacite, &nb_lignes, "PASSIF", 0, 0, 0);
    bilan_ajouter_ligne(sortie, capacite, &nb_lignes, "Capitaux propres et passifs non courants", fonds_propres, 1, 1);
    bilan_ajouter_ligne(sortie, capacite, &nb_lignes, "Capital, reserves et dettes non courantes", capitaux_classe_1, 0, 2);
    bilan_ajouter_ligne(sortie, capacite, &nb_lignes, "Resultat de l exercice", resultat, 0, 2);
    bilan_ajouter_ligne(sortie, capacite, &nb_lignes, "Passifs courants", passifs_courants, 1, 1);
    bilan_ajouter_ligne(sortie, capacite, &nb_lignes, "Tiers crediteurs", tiers_crediteurs, 0, 2);
    bilan_ajouter_ligne(sortie, capacite, &nb_lignes, "Concours bancaires", concours_bancaires, 0, 2);
    bilan_ajouter_ligne(sortie, capacite, &nb_lignes, "TOTAL PASSIF", *total_passif, 1, 0);
 
    /* le controle final : si on a ajoute plus ou moins de 15 lignes, NB_LIGNES_BILAN n est plus a jour (erreur de programmation) */
    if (nb_lignes != NB_LIGNES_BILAN)
        return ERR_ECHEC_GENERATION_RAPPORT;
 
    return ETAT_OK;
}

