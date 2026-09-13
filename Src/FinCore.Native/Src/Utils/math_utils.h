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

/* -------------------------------
    utilitaires numeriques
---------------------------------*/
int64_t math_min_i64(int64_t a, int64_t b);
int64_t math_max_i64(int64_t a, int64_t b);
double math_arrondir(double valeur, int decimales);
#endif
