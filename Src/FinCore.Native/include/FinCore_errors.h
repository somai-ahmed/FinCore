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

    ERR_INCONNU                  = -1,
    ERR_ARGUMENT_INVALIDE        = -2,
    ERR_POINTEUR_NULLE           = -3,
    ERR_SORTIE_DU_MEMOIRE        = -4,
    ERR_SESSION_INVALIDE         = -5,
    ERR_NON_INITIALISE           = -6,
    ERR_TRES_PETIT_BUFFER        = -7,
    ERR_OPERATION_INTERDITE      = -8,
    ERR_IDENTIFIANT_INVALIDE     = -9,
    ERR_ELEMENT_DEJA_EXISTANT    = -10,
    ERR_ELEMENT_INTROUVABLE      = -11,

    /* Erreurs comptables : -100 à -199 */
    ERR_COMPTE_INTROUVABLE          = -100,
    ERR_COMPTE_EXISTE               = -101,
    ERR_COMPTE_INACTIF              = -102,
    ERR_COMPTE_AVEC_ENFANTS         = -103,
    ERR_CODE_COMPTE_INVALIDE        = -104,
    ERR_TYPE_COMPTE_INVALIDE        = -105,
    ERR_CLASSE_COMPTE_INVALIDE      = -106,
    ERR_COMPTE_NON_AUTORISE         = -107,
    ERR_COMPTE_UTILISE              = -108,
    ERR_COMPTE_COLLECTIF_INVALIDE   = -109,
    ERR_COMPTE_AUXILIAIRE_INVALIDE  = -110,
    ERR_COMPTE_RESTREINT            = -111,

    /* Erreurs de journal : -200 à -299 */
    ERR_JOURNAL_INTROUVABLE             = -200,
    ERR_JOURNAL_NON_EQUILIBRE           = -201,
    ERR_JOURNAL_VIDE                    = -202,
    ERR_JOURNAL_DEJA_COMPTABILISE       = -203,
    ERR_LIGNE_JOURNAL_INVALIDE          = -204,
    ERR_JOURNAL_MONTANT_NEGATIF         = -205,
    ERR_JOURNAL_SANS_MONTANT            = -206,
    ERR_JOURNAL_DEBIT_CREDIT_SIMULTANES = -207,
    ERR_DATE_JOURNAL_INVALIDE           = -208,
    ERR_LIBELLE_JOURNAL_MANQUANT        = -209,
    ERR_COMPTE_DEBIT_INVALIDE           = -210,
    ERR_COMPTE_CREDIT_INVALIDE          = -211,
    ERR_JOURNAL_DEJA_EXISTANT           = -212,
    ERR_JOURNAL_PERIODE_INVALIDE        = -213,
    ERR_JOURNAL_PERIODE_FERMEE          = -214,
    ERR_JOURNAL_MONTANT_NUL             = -215,

    /* Erreurs de période : -300 à -399 */
    ERR_PERIODE_INTROUVABLE    = -300,
    ERR_PERIODE_FERMEE         = -301,
    ERR_PERIODE_VERROUILLEE    = -302,
    ERR_PERIODE_CHEVAUCHEMENT  = -303,
    ERR_PLAGE_DATES_INVALIDE   = -304,

    /* Erreurs de rapport : -400 à -499 */
    ERR_ECHEC_GENERATION_RAPPORT = -400,
    ERR_AUCUNE_DONNEE_PERIODE    = -401,

    /* Erreurs du moteur de détection : -500 à -599 */
    ERR_ECHEC_DETECTION         = -500,
    ERR_DONNEES_INSUFFISANTES   = -501,

    /* Erreurs de persistance : -600 à -699 */
    ERR_FICHIER_INTROUVABLE       = -600,
    ERR_ACCES_FICHIER_REFUSE      = -601,
    ERR_FICHIER_CORROMPU          = -602,
    ERR_ECHEC_SERIALISATION      = -603,
    ERR_ECHEC_DESERIALISATION    = -604,
    ERR_VERSION_NON_SUPPORTEE    = -605,
    ERR_ERREUR_ENTREE_SORTIE     = -606
