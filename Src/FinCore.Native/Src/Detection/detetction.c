/*
 * detection.c
 * -----------
 * Chef d'orchestre du moteur de détection.
 *
 * Ce fichier ne fait PAS les calculs statistiques lui-même : il appelle
 * les 4 méthodes de détection (chacune dans son propre fichier : benford.c,
 * doublons.c, montants_ronds.c, valeurs_aberrantes.c), recupere leurs
 * résultats, et les fusionne en un seul tableau qu'il renvoie à l'appelant.
 *
 * Chaque méthode alloue son propre petit tableau de résultats ; ce fichier
 * les recolle ensemble avec realloc() au fur et à mesure.
 */

#include <stdlib.h>  /* malloc, realloc, free pour l allocation dynamique */
#include <string.h> /* pour l usage du memcpy function pour le copiage du un bloc de memoire */


#include "Src/FinCore.Native/include/types.h"
#include "Src/FinCore.Native/include/errors.h"
#include "Src/FinCore.Native/include/session.h"
#include "Src/FinCore.Native/include/detection.h"

/* ---------------------------------------------------------------------------------------------------------------------
 Methodes de detection individuelles                            
Chacune est implemente dans son propre fichier et respecte le
meme contrat que "execute_detection" - fonction :: son idee : elle alloue son tableau en
interne (via malloc/realloc) et le laisse a NULL / 0 en cas d'erreur.

Elles ne font pas partie de l'API publique de la DLL -> donc elles ne sont pas declares dans detection.h, seulement ici 
----------------------------------------------------------------------------------------------------------------------- */

Etat detecter_benford_anomalies(Session *session, PeriodId id_periode, const config_detection *cfg, Resultat_Detection **resultats, size_t *nombre_resultats);

Etat detecter_doublons(Session *session, PeriodId id_periode, const config_detection *cfg, Resultat_Detection **resultats, size_t *nombre_resultats);

Etat detecter_montants_ronds(Session *session, PeriodId id_periode, const config_detection *cfg, Resultat_Detection **resultats, size_t *nombre_resultats);

Etat detecter_valeurs_aberrantes(Session *session, PeriodId id_periode, const config_detection *cfg, Resultat_Detection **resultats, size_t *nombre_resultats);

/* Calcule le rapport statistique complet de Benford (va etre codee dans benford.c) */
Etat calculer_rapport_benford(Session *session, PeriodId id_periode, Rapport_Benford *rapport);
