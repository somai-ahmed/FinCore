#include "math_utils.h"
#include <math.h>

double math_moyenne(const double *valeurs, size_t nombre){
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


double math_ecart_type(const double *valeurs, size_t nombre){
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

double math_ecart_absolu_moyen(const double *valeurs, size_t nombre){
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

/* chercher le minimum ou le maximum du deux nombres entrees */
int64_t math_min_i64(int64_t a, int64_t b){ return (a < b) ? a : b; }

int64_t math_max_i64(int64_t a, int64_t b){ return (a > b) ? a : b; }

double math_arrondir(double valeur, int decimales){
    double facteur = pow(10.0, decimales);
    return round(valeur * facteur) / facteur;
}

int math_premier_chiffre(double valeur){
    valeur = fabs(valeur);
    if (valeur == 0.0) {
        return 0;
    }
    while (valeur < 1.0) {
        valeur *= 10.0;
    }
    while (valeur >= 10.0) {
        valeur /= 10.0;
    }
    return (int)valeur;
}

/* voir Documentation/math/frequence_benford.ipynb */
double math_frequence_benford(int chiffre){
    if (chiffre < 1 || chiffre > 9) {
        return 0.0;
    }
    return log10(1.0 + 1.0 / (double)chiffre);
}

double math_chi_carre(const double *observe, const double *attendu, size_t nombre){ /* χ (chi) est une lettre de l’alphabet grec : χ. */
    /* voir Documentation/math/statistics.ipynb */
    double resultat = 0.0;
    size_t i;

    if (observe == NULL || attendu == NULL || nombre == 0) {
        return 0.0;
    }
    for (i = 0; i < nombre; i++) {
        double diff;
        if (attendu[i] == 0.0) {
            continue;
        }
        diff = observe[i] - attendu[i];
        resultat += (diff * diff) / attendu[i];
    }
    return resultat;
}
