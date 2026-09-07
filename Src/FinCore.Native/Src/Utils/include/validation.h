#ifndef VALIDATION_H
#define VALIDATION_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#include <monnaie.h>

/* Fonctions de validation generiques reutilisees par les couches
 * metier (comptes, ecritures, import Excel, etc.).
 *
 * Convention : chaque fonction retourne un bool (true = valide).
 * Aucune de ces fonctions n'alloue ni ne modifie la chaine
 * d'entree. */

/* ---- chaines de caracteres ---- */

/* true si chaine != NULL et contient au moins un caractere
 * non-espace */
bool valider_chaine_non_vide(const char *chaine);

/* true si la longueur de chaine est comprise entre min et max
 * inclus (bornes en nombre de caracteres, hors '\0') */
bool valider_longueur_chaine(const char *chaine, size_t min, size_t max);

/* true si chaine ne contient que des caracteres alphanumeriques
 * (isalnum) et les caracteres autorises listes dans caracteres_extra
 * (ex: "-_ " pour autoriser tiret, underscore, espace).
 * caracteres_extra peut etre NULL (aucun caractere supplementaire) */
bool valider_caracteres_autorises(const char *chaine, const char *caracteres_extra);

/* ---- codes et references metier ---- */

/* true si code respecte le format d'un code de compte comptable :
 * uniquement des chiffres, longueur entre longueur_min et
 * longueur_max inclus (le plan comptable tunisien utilise
 * generalement des codes a 6-8 chiffres, a ajuster selon le
 * plan comptable retenu dans FinCore_types.h) */
bool valider_code_compte(const char *code, size_t longueur_min, size_t longueur_max);

/* true si reference correspond au format "PREFIXE-XXXXXX" genere
 * par generateur_id_reference (identifiants.h) : prefixe attendu
 * suivi d'un tiret puis uniquement des chiffres */
bool valider_format_reference(const char *reference, const char *prefixe_attendu);

/* ---- valeurs numeriques ---- */

/* true si valeur est comprise entre min et max inclus */
bool valider_plage_i64(int64_t valeur, int64_t min, int64_t max);

/* true si le montant m est valide pour une operation comptable :
 * echelle non negative et, si autoriser_negatif est false,
 * m doit etre >= 0 */
bool valider_montant(monnaie m, bool autoriser_negatif);

#endif

