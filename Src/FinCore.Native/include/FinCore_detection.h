#ifndef FINCORE_DETECTION_H
#define FINCORE_DETETCTION_H

#include "FinCore_types.h"
#include "FinCore_errors.h"


/* ============================================================
* Moteur Comptable FinCore - Moteur de détection
* ============================================================
* Le module Detection permet à FinCore d'identifier les anomalies
* pouvant apparaître dans les écritures et données comptables.
*
* Les anomalies sont classées selon plusieurs méthodes et niveaux
* de sévérité afin de faciliter leur analyse et leur traitement.
*
* Le moteur peut être configuré selon les méthodes de détection
* utilisées et les seuils appliqués lors de l'analyse.
*
* Les résultats retournés permettent d'identifier précisément
* l'écriture, la ligne ou le compte concerné par une anomalie.
*
* Voir documentation/detection-engine.md
* ============================================================ */


typedef enum FinCore_DetectionMethod {
    FinCore_DETECT_BENFORD = 0,
    FinCore_DETECT_DOUBLONS = 1,
    FinCore_DETECT_NOMBRES_RONDS = 2,
    FinCore_DETECT_VALS_ABERRANTES = 3
} FinCore_DetectionMethod;
