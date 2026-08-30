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

typedef enum {
    FINC_DET_BENFORD = 0x01,
    FINC_DET_DOUBLON = 0x02,
    FINC_DET_NOMBRE_ROND = 0x04,
    FINC_DET_VALEUR_ABERRANTE = 0x08,
    FINC_DET_TOUTES = 0x0F
} IndicateurDetection; /* Voir la documentation du fichier pour l'explication du fonctionnement binaire : DOCUMENTATION/Indicateur_Detection_explication_Binaire.ipynb */



/*=========STRUCTURE DES DONNEES=========*/

typedef struct {
    char    code[FINC_LONGUEUR_MAX_CODE_COMPTE];
    char    libelle[FINC_LONGUEUR_MAX_LIBELLE];
    int     type;
    int     categorie;
    char    parent[FINC_LONGUEUR_MAX_CODE_COMPTE];
    double  solde_initial;
    int     est_actif;
} FINC_Compte;


typedef struct {
    char    entry_id[FINC_MAX_ENTRY_ID_LEN];
    char    date[FINC_MAX_DATE_LEN];
    char    account_code[FINC_MAX_ACCOUNT_CODE_LEN];
    char    label[FINC_MAX_LABEL_LEN];
    char    reference[FINC_MAX_REFERENCE_LEN];
    double  debit;
    double  credit;
    char    currency[FINC_MAX_CURRENCY_LEN];
    double  exchange_rate;
    char    period_id[FINC_MAX_PERIOD_ID_LEN];
    char    user_id[FINC_MAX_USER_ID_LEN];
    char    created_at[FINC_MAX_DATETIME_LEN];
} FINC_JournalLine;
