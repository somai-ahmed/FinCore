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

/* ------------------------------------------------------------------ 
                 Fonctions internes ( static functions)                                           
 ------------------------------------------------------------------ */

/*
 * Ajoute les résultats d'une méthode (source, nb_source) à la fin du
 * tableau final (dest, nb_dest), puis libère le petit tableau source
 * (on n'en a plus besoin une fois copié dans le tableau final).
 *
 * realloc() agrandit un bloc mémoire déjà alloué en gardant son contenu ;
 * s'il ne peut pas agrandir sur place, il déplace tout ailleurs et rend
 * la nouvelle adresse (c'est pour ça qu'on ne doit jamais écraser dest
 * avant d'avoir vérifié que realloc n'a pas renvoyé NULL).
 */

static Etat fusionner_resultats(Resultat_Detection **dest, size_t *nb_dest, Resultat_Detection *source, size_t nb_source) {
    if (nb_source == 0) {
        free(source); /* tableau vide éventuel : rien à copier, mais on le libère quand même */
        return ETAT_OK;
    }

    size_t nouvelle_taille = *nb_dest + nb_source;
    Resultat_Detection *nv_dest = realloc(*dest, nouvelle_taille * sizeof(Resultat_Detection));
    if (nv_dest == NULL) {
        free(source);
        return ERR_DETECTION_MEMOIRE;
    }

    /* memcpy copie nb_source structures d'un coup, juste après ce qui existait déjà */
    memcpy(nv_dest + *nb_dest, source, nb_source * sizeof(Resultat_Detection));

    *dest = nv_dest;
    *nb_dest = nouvelle_taille;

    free(source); /* le petit tableau a été recopié, on n'en a plus besoin */
    return ETAT_OK;
}

/* liberer *resultats en cas d'erreur en cours de route, pour ne rien laisser de partiel. */
static void liberer_apres_erreur(Resultat_Detection **resultats, size_t *nombre_resultats) {
    /* liberation */
    /* initialisation a 0 / NULL */
    free(*resultats);
    *resultats = NULL;
    *nombre_resultats = 0;
}
