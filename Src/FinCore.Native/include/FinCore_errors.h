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

typedef enum FinCore_Status {
    FinCore_OK = 0,
