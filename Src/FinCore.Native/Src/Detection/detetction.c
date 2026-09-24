/*
 * detection.c
 * -----------
 * Chef d'orchestre du moteur de détection.
 *
 * Ce fichier ne fait PAS les calculs statistiques lui-même : il appelle
 * les 4 méthodes de détection (chacune dans son propre fichier : benford.c,
 * doublons.c, montants_ronds.c, valeurs_aberrantes.c), récupère leurs
 * résultats, et les fusionne en un seul tableau qu'il renvoie à l'appelant.
 *
 * Chaque méthode alloue son propre petit tableau de résultats ; ce fichier
 * les recolle ensemble avec realloc() au fur et à mesure.
 */

#include <stdlib.h>  /* malloc, realloc, free pour l allocation dynamique */
#include <string.h> /* pour l usage du memcpy function pour le copiage du un bloc de memoire */

#include <Src/FinCore.Native/include/types.h>
#include <Src/FinCore.Native/include/errors.h>
#include <session.h>
#include <detection.h>  /* inclut deja periodes.h et déclare Session */
