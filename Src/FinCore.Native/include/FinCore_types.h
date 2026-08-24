#ifndef FINCORE_TYPES_H
#define FINCORE_TYPES_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <time.h>

typedef struct DATE{
    int jour;
    int mois;
    int annee;
}DATE;

/* ============================================================
 * Moteur comptable FinCore — Définitions des types fondamentaux
 * ============================================================ */

/* Représentation monétaire en virgule fixe : unités entières mineures (ex. millimes)
 * pour éviter les erreurs d'arrondi en virgule flottante dans les calculs comptables. */
typedef int64_t FNC_Monnaie;

#define FNC_Monnaie_Unite 1000  /* 3 decimal places (millimes) */

/*structure interne masquée aux consommateurs */
typedef struct FNC_Session FNC_Session;

/* Identifiants uniques */
typedef uint32_t id_compte;
typedef uint32_t FNC_IdEcriture;
typedef uint32_t FNC_JournalEntryId;
typedef uint32_t FNC_idperiodefiscale;
typedef uint32_t id_ligne;

#define INVALID_ID 0

/* ------------------------------------------------------------
 * Classification des comptes (plan comptable de type PCG tunisien / français)
 * ------------------------------------------------------------ */
/*see documentation/classes_comptabilite.md*/
typedef enum FNC_ClasseCompte {
    FNC_CLASSE_1_CP_ET_PNC = 1,   /* Comptes de capitaux propres et passifs non courants */
    FNC_CLASSE_2_ACTIFS_NON_COURANTS = 2,
    FNC_CLASSE_3_STOCKS = 3,
    FNC_CLASSE_4_TIERS = 4,
    FNC_CLASSE_5_TRESORERIE = 5,
    /* Les charges et produits ne sont pas des classes au sens strict,
       mais on les code ici pour l'uniformité du modèle. */
    FNC_CLASSE_6_CHARGES = 6,
    FNC_CLASSE_7_PRODUITS = 7
} FNC_ClasseCompte;

typedef enum TypeCompte {
    FNC_COMPTE_ACTIF = 1,          
    FNC_COMPTE_PASSIF = 2,    
    FNC_COMPTE_CAPITAUX = 3,    
    FNC_COMPTE_PRODUIT = 4,   
    FNC_COMPTE_CHARGE = 5    
} FNC_TypeCompte;

typedef enum FNC_SoldeNormal {
    FNC_SOLDE_DEBITEUR = 0,        /* DEBIT */
    FNC_SOLDE_CREDITEUR = 1        /* CREDIT */
} FNC_SoldeNormal;

typedef struct Compte {
    FNC_IdCompte     id;
    char                 code[16];           /* ex. "512000" */
    char                 nom[128];
    FNC_ClasseCompte classe;
    FNC_TypeCompte   type;
    FNC_SoldeNormal  solde_normal;
    FNC_IdCompte     parent_id;          /* 0 si racine */
    int                  est_active;
}Compte;

/*--------------------------------------------------------------
    l implementation des structures pour la journal
---------------------------------------------------------------*/
typedef struct ligne_journal{
    id_ligne id;
    id_compte compte_id;
    FNC_Monnaie Debit;
    FNC_Monnaie credit ;
    char libelle[256] ;
}ligne_journal;

typedef struct FinCore_Ecriture {
    FNC_IdEcriture     id;
    FNC_IdPeriode      periode_id;
    DATE                   date[11];
    char                   reference[32];
    char                   description[256];
    LigneJournal*  lignes;
    size_t                 nombre_lignes;
    int                    est_validee;     /* 0 = brouillon, 1 = validée */
} FinCore_Ecriture;

typedef enum status_periode_fiscale{
    PERIODE_FISCALE_OUVERTE = 1 ;
    PERIODE_FISCALE_CLOTURE = 2 ;
    PERIODE_FISCALE_VEROUILLEE = 3 ;
}statsus_periode_fiscale;

typedef struct prop_periode_fiscale {
    FNC_IdPeriode    id;
    char nom[64];
    DATE date_debut;
    DATE date_fin[11];
    status_periode_fiscale statut;
}prop_periode_fiscale;

/*--------------------------------------------------------------
    l implementation des structures pour la balance
---------------------------------------------------------------*/

/*implementation d une structure pour appeler les champs du la structure compte*/
typedef struct ref_compte{
    id_compte id;
    char code [16]
    char nom[256];
}ref_compte;

typedef struct ligne_balance {
    ref_compte compte;
    FNC_Monnaie total_debit;
    FNC_Monnaie total_credit;
    FNC_Monnaie solde_debit;
    FNC_Monnaie solde_credit;
}LigneBalance;

typedef struct Entree_GrandLivre {
    DATE date;
    char reference[32];
    char libelle[256];
    FinCore_Monnaie debit;
    FinCore_Monnaie credit;
    FinCore_Monnaie solde_cumule;
}Entree_GrandLivre;
