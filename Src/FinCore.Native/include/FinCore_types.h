#ifndef FINCORE_TYPES_H
#define FINCORE_TYPES_H

#include <stdint.h>
#include <stddef.h>

/* ============================================================
 * FinCore Accounting Engine - Core Type Definitions
 * ============================================================ */

/* Fixed-point money representation: integer minor units (e.g. millimes)
 * to avoid floating point rounding errors in accounting calculations. */
typedef int64_t FinCore_Monnaie;

#define FinCore_Monnaie_Size 1000  /* 3 decimal places (millimes) */

