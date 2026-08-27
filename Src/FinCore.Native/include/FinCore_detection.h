#ifndef FINCORE_DETECTION_H
#define FINCORE_DETETCTION_H

#include "FinCore_types.h"
#include "FinCore_errors.h"

typedef enum FinCore_DetectionMethod {
    FinCore_DETECT_BENFORD = 0,
    FinCore_DETECT_DOUBLONS = 1,
    FinCore_DETECT_NOMBRES_RONDS = 2,
    FinCore_DETECT_VALS_ABERRANTES = 3
} FinCore_DetectionMethod;
