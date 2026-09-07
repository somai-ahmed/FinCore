#ifndef IDENTIFIANTS_H
#define IDENTIFIANTS_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

/* Generateur d'identifiants uniques pour les entites du moteur
 * (comptes, ecritures, etc.)*/
typedef struct {
    int64_t compteur;
} generateur_id;


#endif
