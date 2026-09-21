/*
 * Génère le compte de résultat (charges et produits) d'un exercice.
 *
 * idee : ici on ne relit pas le journal :: On reçoit, pour chaque compte
 * le total de ses débits et le total de ses crédits sur la période
 * (c'est exactement ce que calcule la balance). 
 * On garde seulement les comptes de classe 6 (charges) et de classe 7 (produits)
 * on les range  par rubrique, puis on calcule les résultats intermédiaires :
 * exploitation, financier, exceptionnel, avant impot etc...
 */

#include <stdio.h>   /* snprintf : écrire du texte dans un tableau sans déborder */
#include <string.h>  /* memset, strlen, strncmp : outils pour la mémoire et les chaînes */

#include "Src/FinCore.Native/include/types.h"
#include "Src/FinCore.Native/include/errors.h" 
#include <comptes.h>
#include <monnaie.h>
#include <reports.h>

/* ------------------------------------------------------------------
   Table de correspondance : début du code du compte -> rubrique   
  ------------------------------------------------------------------ */

/* Une règle = "si le code commence par ce préfixe, alors c'est cette rubrique" */
typedef struct {
    const char *prefixe;   /* les premiers chiffres du code du compte */
    RubriqueCR rubrique;   /* la rubrique du compte de résultat visée */
} RegleRubrique;

/*
              MATRICE DES VALEURS : (DATASET)
 * Quand plusieurs règles correspondent, on garde la plus précise (le préfixe
 * le plus long). Exemple : le compte 686 tombe dans "686" (financier)
 * et non dans "68" (exploitation).
 * Si tes comptes suivent le SCE tunisien plutôt que le PCG, c'est ICI
 * (et seulement ici) qu'il faut ajuster les préfixes
 */
static const RegleRubrique REGLES_RUBRIQUES[] = {
    /* Charges d'exploitation */
    { "60", CR_CHARGES_EXPLOITATION },
    { "61", CR_CHARGES_EXPLOITATION },
    { "62", CR_CHARGES_EXPLOITATION },
    { "63", CR_CHARGES_EXPLOITATION },
    { "64", CR_CHARGES_EXPLOITATION },
    { "65", CR_CHARGES_EXPLOITATION },
    { "68", CR_CHARGES_EXPLOITATION },

    /* Charges financières et exceptionnelles */
    { "66",  CR_CHARGES_FINANCIERES },
    { "686", CR_CHARGES_FINANCIERES },
    { "67",  CR_CHARGES_EXCEPTIONNELLES },
    { "687", CR_CHARGES_EXCEPTIONNELLES },

    /* Impôt sur les bénéfices */
    { "69", CR_IMPOT_BENEFICES },

    /* Produits d'exploitation */
    { "70", CR_PRODUITS_EXPLOITATION },
    { "71", CR_PRODUITS_EXPLOITATION },
    { "72", CR_PRODUITS_EXPLOITATION },
    { "73", CR_PRODUITS_EXPLOITATION },
    { "74", CR_PRODUITS_EXPLOITATION },
    { "75", CR_PRODUITS_EXPLOITATION },
    { "78", CR_PRODUITS_EXPLOITATION },
    { "79", CR_PRODUITS_EXPLOITATION },

    /* Produits financiers et exceptionnels */
    { "76",  CR_PRODUITS_FINANCIERS },
    { "786", CR_PRODUITS_FINANCIERS },
    { "77",  CR_PRODUITS_EXCEPTIONNELS },
    { "787", CR_PRODUITS_EXCEPTIONNELS }
};
