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

#define FinCore_Monnaie_Size 1000  /* 3 decimal places (millimes) */

/*structure interne masquée aux consommateurs */
typedef struct FinCore_Session FinCore_Session;

