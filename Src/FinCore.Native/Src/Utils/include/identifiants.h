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

/* initialise un generateur a partir d'une valeur de depart
 * (utile pour reprendre une sequence depuis le fichier JSON
 * de persistance au demarrage) */
generateur_id initiation_generateur_id(int64_t valeur_depart);

/* retourne le prochain identifiant numerique unique (monotone croissant) */
int64_t generateur_id_suivant(generateur_id *gen);

/* genere une reference lisible du type "PREFIXE-000123" dans tampon.
 * - prefixe    : ex. "CPT", "ECR", "FACT"
 * - largeur_min: nombre minimum de chiffres (complete avec des zeros)
 * retourne false si le tampon est trop petit ou les parametres invalides */
bool generateur_id_reference(generateur_id *gen, const char *prefixe, int largeur_min, char *tampon, size_t taille_tampon);

#endif
