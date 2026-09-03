/* Date utility functions and definitions for handling financial and accounting-related date operations */

/* Centralized date management utilities for journal entries, accounting periods, fiscal years, and financial reports
 * Ensures consistent, reliable, and well-tested date handling across the application */

#ifndef DATES_H
#define DATES_H


#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

typedef struct DATE {
    int32_t annee;
    int32_t mois;
    int32_t jour;
} DATE;

typedef enum {
    DATE_FORMAT_ISO,
    DATE_FORMAT_FR
}DATE_FORMAT;

#define LONGEUR_CHAINE_DATE 11 /* 4 POUR ANNNEE , 2 POUR JOUR , 2 POUR MOIS , 3 tirets ou SLACH '/' */

#endif
