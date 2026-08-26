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

    /* Generic / session errors: -1 to -99 */
    FinCore_ERR_UNKNOWN              = -1,
    FinCore_ERR_INVALID_ARGUMENT     = -2,
    FinCore_ERR_NULL_POINTER         = -3,
    FinCore_ERR_OUT_OF_MEMORY        = -4,
    FinCore_ERR_INVALID_SESSION      = -5,
    FinCore_ERR_NOT_INITIALIZED      = -6,
    FinCore_ERR_BUFFER_TOO_SMALL     = -7,

    /* Accounting errors: -100 to -199 */
    FinCore_ERR_ACCOUNT_NOT_FOUND    = -100,
    FinCore_ERR_ACCOUNT_EXISTS       = -101,
    FinCore_ERR_ACCOUNT_INACTIVE     = -102,
    FinCore_ERR_ACCOUNT_HAS_CHILDREN = -103,
    FinCore_ERR_INVALID_ACCOUNT_CODE = -104,

    /* Journal errors: -200 to -299 */
    FinCore_ERR_JOURNAL_NOT_FOUND    = -200,
    FinCore_ERR_JOURNAL_UNBALANCED   = -201,
    FinCore_ERR_JOURNAL_EMPTY        = -202,
    FinCore_ERR_JOURNAL_ALREADY_POSTED = -203,
    FinCore_ERR_JOURNAL_INVALID_LINE = -204,
    FinCore_ERR_JOURNAL_NEGATIVE_AMOUNT = -205,
