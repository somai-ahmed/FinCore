#ifndef FINCORE_ERRORS_H
#define FINCORE_ERRORS_H

/* ============================================================
 * Moteur Comptable FinCore - Codes d'erreur
 * ============================================================
 * Toutes les fonctions publiques de l'API retournent un
 * Status. 0 signifie toujours le succès ; les valeurs
 * négatives sont des erreurs, regroupées par module en blocs
 * de -100 afin que VB.NET puisse mapper les plages à des
 * types d'exceptions.
 * ============================================================ */

/* --------------------------------------------------------------
    FinCore peut envisager pluseieurs erreurs : ces erreurs sont
    clasé sur plusieures modules , dont chaque erreur appartient
    a un grand classe des erreur : 
    les codes des erreurs sont presentes comme des intervalles
    c'est le cas ou : 
    on definit l'erreur par : 
        |    INTERVALLE     | MODULE    |  ERREUR PREVUE     |

        NB : les erreurs sont ppresente en nombres NEGATIFS (le 
        correcte codage des codes des erreurs)

    voir documentation/erreurs.md
    -------------------------------------------------------------*/
        

typedef enum FinCore_Etat {
    ETAT_OK = 0,

    /* Erreurs générales : -1 à -99 */
    ERR_INCONNU              = -1,
    ERR_ARGUMENT_INVALIDE     = -2,
    ERR_POINTEUR_NULLE         = -3,
    ERR_SORTIE_DU_MEMOIRE        = -4,
    ERR_SESSION_INVALIDE      = -5,
    ERR_NON_INITIALISE      = -6,
    ERR_TRES_PETIT_BUFFER     = -7,

    /* Erreurs comptables : -100 à -199 */
    ERR_COMPTE_INTROUVABLE    = -100,
    ERR_COMPTE_EXISTE         = -101,
    ERR_COMPTE_INACTIF        = -102,
    ERR_COMPTE_AVEC_ENFANTS   = -103,
    ERR_CODE_COMPTE_INVALIDE  = -104,

    /* Erreurs de journal : -200 à -299 */
    ERR_JOURNAL_INTROUVABLE       = -200,
    ERR_JOURNAL_NON_EQUILIBRE     = -201,
    ERR_JOURNAL_VIDE              = -202,
    ERR_JOURNAL_DEJA_COMPTABILISE = -203,
    ERR_LIGNE_JOURNAL_INVALIDE    = -204,
    ERR_JOURNAL_MONTANT_NEGATIF   = -205,

    /* Erreurs de période : -300 à -399 */
    FinCore_ERR_PERIODE_INTROUVABLE    = -300,
    FinCore_ERR_PERIODE_FERMEE         = -301,
    FinCore_ERR_PERIODE_VERROUILLEE    = -302,
    FinCore_ERR_PERIODE_CHEVAUCHEMENT  = -303,
    FinCore_ERR_PLAGE_DATES_INVALIDE   = -304,

    /* Erreurs de rapport : -400 à -499 */
    FinCore_ERR_ECHEC_GENERATION_RAPPORT = -400,
    FinCore_ERR_AUCUNE_DONNEE_PERIODE    = -401,
