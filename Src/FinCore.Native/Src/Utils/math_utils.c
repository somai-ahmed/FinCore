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
