/* Fonctions et definitions utilitaires pour la gestion des dates
 * liees aux operations comptables et financieres */

/* Gestion centralisee des dates pour les ecritures comptables
 * les periodes comptables les exercices fiscaux et les rapports financiers
 * ce qui garentit une gestion des dates coherente fiable et testee
 * dans l ensemble de l application */


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

#define FEVRIER_JOURS_NORMAL    28
#define FEVRIER_JOURS_BISSEXTILE 29

/*----------------------------------
    FONCTIONS DE VERIFICATION
-----------------------------------*/
bool est_annee_bissextile(int32_t annee);
bool date_valide(DATE d);

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
bool analyser_date(const char *date_en_texte , DATE_FORMAT format , DATE *resultat);
bool formater_date(DATE date , DATE_FORMAT format , char *buffer , int32_t taille_buffer);

#endif
