#include "math_utils.h"
#include <math.h>

double math_moyenne(const double *valeurs, size_t nombre)
{
    double somme = 0.0;
    size_t i;

    if (valeurs == NULL || nombre == 0) {
        return 0.0;
    }
    for (i = 0; i < nombre; i++) {
        somme += valeurs[i];
    }
    return somme / (double)nombre;
}


double math_ecart_type(const double *valeurs, size_t nombre)
{
    double moyenne;
    double somme_carres = 0.0;
    size_t i;

    if (valeurs == NULL || nombre < 2) {
        return 0.0;
    }
    moyenne = math_moyenne(valeurs, nombre);
    for (i = 0; i < nombre; i++) {
        double ecart = valeurs[i] - moyenne;
        somme_carres += ecart * ecart;
    }
    return sqrt(somme_carres / (double)(nombre - 1));
}

double math_ecart_absolu_moyen(const double *valeurs, size_t nombre)
{
    double moyenne;
    double somme_abs = 0.0;
    size_t i;

    if (valeurs == NULL || nombre == 0) {
        return 0.0;
    }
    moyenne = math_moyenne(valeurs, nombre);
    for (i = 0; i < nombre; i++) {
        somme_abs += fabs(valeurs[i] - moyenne);
    }
    return somme_abs / (double)nombre;
}
