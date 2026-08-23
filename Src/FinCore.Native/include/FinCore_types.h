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
typedef enum FinCore_classes_comptabilite {
    FinCore_CLASS_1_CP_&_PNC = 1,   /* Comptes de capitaux propres et passifs non courants */
    FinCore_CLASS_2_ANC = 2,
    FinCore_CLASS_3_STOCKS = 3,
    FinCore_CLASS_4_TIERS = 4,
    FinCore_CLASS_5_FINANCIER = 5,
    /*CHARGES ET PRODUITS ne sont pas des classes mais on le met ici pour le coder*/
    FinCore_CLASS_6_CHARGES = 6,
    FinCore_CLASS_7_PRODUITS = 7
} FinCore_classes_comptabilite ;

