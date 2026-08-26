#ifndef FinCore_ERRORS_H
#define FinCore_ERRORS_H

/* ============================================================
 * Moteur Comptable FinCore - Codes d'erreur
 * ============================================================
 * Toutes les fonctions publiques de l'API retournent un
 * FinCore_Status. 0 signifie toujours le succès ; les valeurs
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
    FinCore_OK = 0,

    /* Erreurs générales : -1 à -99 */
    FinCore_ERR_INCONNU              = -1,
    FinCore_ERR_ARGUMENT_INVALIDE     = -2,
    FinCore_ERR_POINTEUR_NULLE         = -3,
    FinCore_ERR_SORTIE_DU_MEMOIRE        = -4,
    FinCore_ERR_SESSION_INVALIDE      = -5,
    FinCore_ERR_NON_INITIALISE      = -6,
    FinCore_ERR_TRES_PETIT_BUFFER     = -7,

    /* Erreurs comptables : -100 à -199 */
    FinCore_ERR_COMPTE_INTROUVABLE    = -100,
    FinCore_ERR_COMPTE_EXISTE         = -101,
    FinCore_ERR_COMPTE_INACTIF        = -102,
    FinCore_ERR_COMPTE_AVEC_ENFANTS   = -103,
    FinCore_ERR_CODE_COMPTE_INVALIDE  = -104,

    /* Erreurs de journal : -200 à -299 */
    FinCore_ERR_JOURNAL_INTROUVABLE       = -200,
    FinCore_ERR_JOURNAL_NON_EQUILIBRE     = -201,
    FinCore_ERR_JOURNAL_VIDE              = -202,
    FinCore_ERR_JOURNAL_DEJA_COMPTABILISE = -203,
    FinCore_ERR_LIGNE_JOURNAL_INVALIDE    = -204,
    FinCore_ERR_JOURNAL_MONTANT_NEGATIF   = -205,
