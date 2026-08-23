#ifndef FINCORE_TYPES_H
#define FINCORE_TYPES_H

#include <stdint.h>
#include <stddef.h>

/* ============================================================
 * Moteur comptable FinCore — Définitions des types fondamentaux
 * ============================================================ */

/* Représentation monétaire en virgule fixe : unités entières mineures (ex. millimes)
 * pour éviter les erreurs d'arrondi en virgule flottante dans les calculs comptables. */
typedef int64_t FinCore_Monnaie;

#define FinCore_Monnaie_Unite 1000  /* 3 decimal places (millimes) */

/*structure interne masquée aux consommateurs */
typedef struct FinCore_Session FinCore_Session;

/* Identifiants uniques */
typedef uint32_t FinCore_AccountId;
typedef uint32_t FinCore_JournalEntryId;
typedef uint32_t FinCore_PeriodId;
typedef uint32_t FinCore_LineId;

#define INVALID_ID 0

/* ------------------------------------------------------------
 * Classification des comptes (plan comptable de type PCG tunisien / français)
 * ------------------------------------------------------------ */
/*see documentation/classes_comptabilite.md*/
typedef enum FinCore_ClasseCompte {
    FinCore_CLASSE_1_CP_ET_PNC = 1,   /* Comptes de capitaux propres et passifs non courants */
    FinCore_CLASSE_2_ACTIFS_NON_COURANTS = 2,
    FinCore_CLASSE_3_STOCKS = 3,
    FinCore_CLASSE_4_TIERS = 4,
    FinCore_CLASSE_5_TRESORERIE = 5,
    /* Les charges et produits ne sont pas des classes au sens strict,
       mais on les code ici pour l'uniformité du modèle. */
    FinCore_CLASSE_6_CHARGES = 6,
    FinCore_CLASSE_7_PRODUITS = 7
} FinCore_ClasseCompte;


