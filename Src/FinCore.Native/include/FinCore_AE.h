#ifndef FinCore_AE_H
#define FinCore_AE_H

#include "FinCore_types.h"
#include "FinCore_errors.h"
#include "FinCore_detection.h"



/*=======definition des constants========*/
#define LONGUEUR_MAX_LIBELLE             256
#define LONGUEUR_MAX_CODE_COMPTE         16
#define LONGUEUR_MAX_DEVISE              4
#define LONGUEUR_MAX_ID_ECRITURE         32
#define LONGUEUR_MAX_ID_PERIODE          16
#define LONGUEUR_MAX_ID_UTILISATEUR      32
#define LONGUEUR_MAX_REFERENCE           64
#define LONGUEUR_MAX_SECTION             64
#define LONGUEUR_MAX_DATE                11
#define LONGUEUR_MAX_DATE_HEURE          20

#define MAX_SESSIONS                     16
#define MAX_COMPTES                      1000
#define MAX_PERIODES                     50
#define MAX_LIGNES_JOURNAL               100000
#define MAX_ECRITURES                    50000
#define MAX_DETECTIONS                   10000
#define MAX_LIGNES_RAPPORT               50000

#define BENFORD_ECHANTILLON_MIN          300

/*=======les enumerations========*/
typedef enum {
    OK = 0,
    ERREUR_ENTREE_INVALIDE = -1,
    ERREUR_MEMOIRE_INSUFFISANTE = -2,
    ERREUR_COMPTE_INTROUVABLE = -3,
    ERREUR_ECRITURE_NON_EQUILIBREE = -4,
    ERREUR_PERIODE_FERMEE = -5,
    ERREUR_CLE_DUPLIQUEE = -6,
    ERREUR_ENTREE_SORTIE = -7,
    ERREUR_DETECTION = -8,
    ERREUR_SESSION_INTROUVABLE = -9,
    ERREUR_PERIODE_INTROUVABLE = -10,
    ERREUR_ECRITURE_INTROUVABLE = -11,
    ERREUR_COMPTE_UTILISE = -12,
    ERREUR_MONTANT_NUL = -13,
    ERREUR_DATE_INVALIDE = -14,
    ERREUR_DONNEES_INSUFFISANTES = -15
} ETAT;

typedef enum {
    ACTIF = 1,
    PASSIF = 2,
    CHARGE = 3,
    PRODUIT = 4
} TypeCompte;

typedef enum {
    FINC_CAT_BALANCE = 0,
    FINC_CAT_BILAN = 1,
    FINC_CAT_RESULTAT = 2
} Categorie_Compte;

typedef enum {
    FINC_PERIODE_OUVERTE = 0,
    FINC_PERIODE_FERMEE = 1,
    FINC_PERIODE_VERROUILLEE = 2
} ETAT_PERIODE;


/*
*    INDICATEUR DU DETECTION
*    
* Chaque valeur correspond a un bit :
* 
*    0x01 -> Benford
*    0x02 -> Doublons
*    0x04 -> Nombres ronds
*    0x08 -> Valeurs aberrantes
*
*    0x0F -> Toutes les detections
*
*    VOIR :
* Documentation/Indicateur_Detection_explication_Binaire.ipynb
*/
    
typedef enum {
    FINC_DET_BENFORD = 0x01,
    FINC_DET_DOUBLON = 0x02,
    FINC_DET_NOMBRE_ROND = 0x04,
    FINC_DET_VALEUR_ABERRANTE = 0x08,
    FINC_DET_TOUTES = 0x0F
} IndicateurDetection;

/*=========STRUCTURE DES DONNEES=========*/

/*
 * Structure representant un compte comptable.
 */
typedef struct {
    char    code[ LONGUEUR_MAX_CODE_COMPTE];
    char    libelle[LONGUEUR_MAX_LIBELLE];
    int     type;
    int     categorie;
    char    parent[LONGUEUR_MAX_CODE_COMPTE];
    double  solde_initial;
    int     est_actif;
}Compte;



typedef struct LigneJournal{
    char    id_ecriture[LONGUEUR_MAX_ID_ECRITURE];
    DATE    date;
    char    code_compte[FINC_LONGUEUR_MAX_CODE_COMPTE];
    char    libelle[FINC_LONGUEUR_MAX_LIBELLE];
    char    reference[FINC_LONGUEUR_MAX_REFERENCE];
    double  debit;
    double  credit;
    char    devise[FINC_LONGUEUR_MAX_DEVISE];
    double  taux_change;
    char    id_periode[FINC_LONGUEUR_MAX_ID_PERIODE];
    char    id_utilisateur[FINC_LONGUEUR_MAX_ID_UTILISATEUR];
    char    date_creation[FINC_LONGUEUR_MAX_DATE_HEURE];
} LigneJournal;
