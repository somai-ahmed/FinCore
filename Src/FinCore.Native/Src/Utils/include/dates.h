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


/*----------------------------------
    FONCTIONS DE VERIFICATION
-----------------------------------*/
bool date_valide(DATE d);
bool est_annee_bissextile(int32_t annee);
int32_t obtenir_nbr_jours_mois(int32_t annee , int32_t mois); /*pour la verification du mois FEVRIER 28 ou 29*/

/*------------------------------------
    FONCTIONS DE COMPARISON            
-------------------------------------*/
int comparer_dates(DATE d1, DATE d2); /*si d1>d2 : fonction retourne >0 sinon <0 :: si la meme date on recoit un ZERO 0 */
bool date_inclus_dans_periode (DATE d , DATE debut_periode , DATE fin_periode);

/*------------------------------------
   FONCTIONS ARITHMETHIQUE SUR DATE            
-------------------------------------*/
DATE ajouter_jours_au_date(DATE d,int32_t jours);
DATE difference_date(DATE d1 ,DATE d2);
int32_t date_en_jdn(DATE d);
DATE jdn_en_date (int32_t jdn);

/* -------------------------------------------------
            ANALYSE -- FORMATAGE
    le principe : la fonction du type booleen :
    elle retourne :
 * "TRUE" en cas de succès, "FALSE" en cas d'échec.
--------------------------------------------------*/
#endif
