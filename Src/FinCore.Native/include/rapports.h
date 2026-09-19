/*
 * rapports.h -- rapports financiers construits a partir d'une Session
 *
 * Chaque fonction Reports_Build* remplit une structure fournie par l'appelant ;
 * les lignes sont allouees dynamiquement par le moteur. Elles doivent etre
 * liberees avec la fonction Reports_Free* correspondante (qui remet egalement
 * la structure a zero).
 *
 * Arguments de periode : `debut` et `fin` sont tous deux inclusifs. NULL signifie
 * qu'il n'y a pas de limite de ce cote, ainsi que (NULL, NULL) couvre
 * l'ensemble du journal.
 *
 * Les montants sont de type FNC_Monnaie (int64, echelle fixe a 3 decimales).
 * Les lignes sont des copies : elles restent valides meme si la session est
 * modifiee par la suite.
 *
 * Compatible avec ctypes : structures simples, entiers a largeur fixe,
 * sans champs de bits ni bool.
 */


#ifndef RAPPORTS_H
#define RAPPORTS_H

#include <stddef.h>
#include <stdint.h>

#include "types.h"
#include "errors.h"
#include "dates.h"
#include "session.h"

/* ## FNC cad FINCANCE_CORE , pour des noms plus specifiques des vairables ## */

/* Le texte copie dans les lignes des rapports est tronque pour tenir
 * dans les champs prevus (toujours termine par NULL). */
#define FNC_RAPPORT_CODE_LEN 16
#define FNC_RAPPORT_NOM_LEN 128
#define FNC_RAPPORT_LIBELLE_LEN 256

/* ------------------------------------------------------------------ */
/* Balance                                                            */
/* ------------------------------------------------------------------ */

typedef struct {
    int32_t compte_id;
    char code[FNC_RAPPORT_CODE_LEN];
    char nom[FNC_RAPPORT_NOM_LEN];
    FNC_ClasseCompte classe;
    monnaie total_debit;
    monnaie total_credit;
    monnaie solde_debiteur;  /* max(debit - credit, 0) */
    monnaie solde_crediteur; /* max(credit - debit, 0) */
} LigneBalance;

typedef struct {
    LigneBalance *lignes; /* triees par code ; comptes sans mouvement omis */
    size_t nb_lignes;
    monnaie total_debit;
    monnaie total_credit;
    monnaie total_solde_debiteur;
    monnaie total_solde_crediteur;
    int32_t est_equilibree; /* 1 si total_debit == total_credit, sinon 0 */
} Balance;

Etat Reports_BuildBalance(const Session *session,const DATE *debut, const DATE *fin, Balance *sortie);
void Reports_FreeBalance(Balance *balance);

/* ------------------------------------------------------------------ */
/* Grand livre (une seule compte)                                     */
/* ------------------------------------------------------------------ */

typedef struct {
    DATE date;
    int32_t ecriture_id;
    char libelle[FNC_REPORT_LIBELLE_LEN];
    monnaie debit;
    monnaie credit;
    monnaie solde; /* solde cumule apres cette ligne : debit - credit */
} LigneGrandLivre;

typedef struct {
    int32_t compte_id;
    char code[FNC_REPORT_CODE_LEN];
    char nom[FNC_REPORT_NOM_LEN];
    monnaie solde_ouverture; /* solde juste avant `debut` (debit - credit) */
    LigneGrandLivre *lignes;     /* dans l'ordre chronologique */
    size_t nb_lignes;
    monnaie total_debit;
    monnaie total_credit;
    monnaie solde_cloture;   /* solde_ouverture + total_debit - total_credit */
} GrandLivre;

Etat Reports_BuildGrandLivre(const Session *session, int32_t compte_id, const DATE *debut, const DATE *fin, GrandLivre *sortie);
void Reports_FreeGrandLivre(GrandLivre *grand_livre);

/* ------------------------------------------------------------------ */
/* Bilan (situation patrimoniale a une date donnee)                   */
/* ------------------------------------------------------------------ */

typedef struct {
    int32_t compte_id;
    char code[FNC_RAPPORT_CODE_LEN];
    char nom[FNC_RAPPORT_NOM_LEN];
    FNC_SectionBilan section;
    monnaie montant; /* presente comme un montant positif dans son cote */
} LigneBilan;

typedef struct {
    DATE date_arrete;
    LigneBilan *lignes;
    size_t nb_lignes;
    monnaie total_actif;
    monnaie total_passif;
    monnaie resultat_net; /* classes 7 - 6, deja inclus dans total_passif */
    int32_t est_equilibre;    /* 1 si total_actif == total_passif, sinon 0 */
} Bilan;

/* date_arrete == NULL -> fin de l'exercice comptable selon la configuration
 * de la session. */
Etat creer_bilan(const Session *session, const DATE *date_arrete, Bilan *sortie);
void liberer_bilan(Bilan *bilan);

#endif  
