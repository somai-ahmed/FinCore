#ifndef MATH_UTILS_H
#define MATH_UTILS_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

/* -------------------------------
    statistiques de base
---------------------------------*/
double math_moyenne(const double *valeurs, size_t nombre);
double math_ecart_type(const double *valeurs, size_t nombre);
double math_ecart_absolu_moyen(const double *valeurs, size_t nombre);

#endif
