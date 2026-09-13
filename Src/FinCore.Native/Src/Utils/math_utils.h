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

/* -------------------------------
    utilitaires pour Benford
---------------------------------*/
/* Premier chiffre significatif (1-9) d'un nombre, signe ignore.
 * Retourne 0 si valeur == 0. */
int math_premier_chiffre(double valeur);
/* Frequence attendue du chiffre d (1-9) selon la loi de Benford :
 * log10(1 + 1/d) */
double math_frequence_benford(int chiffre);

/* Test du chi-carre entre frequences observees et attendues.
 * "nombre" = taille des deux tableaux (doivent correspondre). */
double math_chi_carre(const double *observe, const double *attendu, size_t nombre);

#endif
#endif
