#ifndef FINCORE_DETECTION_H
#define FINCORE_DETETCTION_H

#include "FinCore_types.h"
#include "FinCore_errors.h"


/* ============================================================
* Moteur Comptable FinCore - Moteur de détection
* ============================================================
* Le module Detection permet à FinCore d'identifier les anomalies
* pouvant apparaître dans les écritures et données comptables.
*
* Les anomalies sont classées selon plusieurs méthodes et niveaux
* de sévérité afin de faciliter leur analyse et leur traitement.
*
* Le moteur peut être configuré selon les méthodes de détection
* utilisées et les seuils appliqués lors de l'analyse.
*
* Les résultats retournés permettent d'identifier précisément
* l'écriture, la ligne ou le compte concerné par une anomalie.
*
* Voir documentation/detection-engine.md
* ============================================================ */


typedef enum Meth_Detection {
    FinCore_DETECT_BENFORD = 0,
    FinCore_DETECT_DOUBLONS = 1,
    FinCore_DETECT_NOMBRES_RONDS = 2,
    FinCore_DETECT_VALS_ABERRANTES = 3
} Meth_Detection;

/*Les niveaux de gravite sont numerotes du niveau le plus sain 
    au niveau le plus dangereux*/
typedef enum Gravite {
   FinCore_GRAVITE_INFO     = 0,
   FinCore_GRAVITE_FAIBLE   = 1,
   FinCore_GRAVITE_MOYENNE  = 2,
   FinCore_GRAVITE_ELEVEE   = 3,
   FinCore_GRAVITE_CRITIQUE = 4
} Gravite;

typedef struct Resultat_Detection{
    Meth_Detection methode;
    Gravite gravite;
    id_ligne id_ligne; /*extrait de la fichier FinCore_types.h -- << typedef uint32_t id_ligne >> */
    id_compte id_compte; /*extrait de la fichier FinCore_types.h -- << typedef uint32_t id_compte >> */
    char description[256];
    double score ; /*la detection du resultat va etre analyse sur des bases mathematiques et des formules precis sous forme d'un score
    puis on va classer ceci selon le score obtenu*/
}Resultat_Detection;

/*voir l'explication du theoreme Benford en Documentation/Benford_strategie.ipynb*/
typedef struct StatChiffreBenford {
    int numero; /* intervalle [0,9] */
    double freq_attendu; /* Proportion attendue selon Benford */
    double freq_observe; /* Proportion réelle dans le jeu de données */
    int nombre_observe;
} StatChiffreBenford;

/*pour le rapport on a besoin des statistiques*/
/*l'appelle du structure "StatChiffreBenford" pour la designiation du rapport*/
typedef struct Rapport_Benford{
    StatChiffreBenford stat[9] ;
    double chiffre_carree;
    double ecart_absolu_moyen;
    int taille_echantillon;
    int estnormal ; /*variable booleenne peut prendre seulemnt 0 ou 1 comme un valeur*/
}Rapport_Benford;

typedef struct config_detection{
    int benford_est_active ;
    int doublons_est_active;
    int nombres_ronds_active;
    double seuil_ecart_type_extreme;    /* ex :3,0 sigma */
    double seuil_ecart_nombres_ronds; /*ex: montants divisibles par 1000*/
    int fenetre_jours_doublon; /*période de recherche des doublons*/
}config_detection;

/*pour le fonctionnement d'une maniere propre du programme , on necessite l'integration du l'API du detection*/
/*cad l API aka "APPLICATION PROGRAMMING INTERFACE" : l'interface entre le programme et le moteur du detection*/
/*dans ce cas , la session comptable va etre analyse et passe par l'API du detection pour on conclure a la fin
    le RESULTAT puis le renvoi au programme principal*/

/* voir Documentation/API_detection.ipynb*/

/*----------------------------------------------------------------------
                        API DU DETECTION
    ------------------------------------------------------------------*/

/* Remplit config avec des valeurs par défaut 
    --> Cette fonction ne peut jamais etre FRAUDE*/
void detection_config_par_defaut(config_detection* cfg);

/* Exécute toutes les méthodes de détection activées sur les écritures
    comptables validées pendant la période donnée
    Les résultats sont alloués en interne
    l'appelant doit appeler FinCore_LibererResultatsDetection lorsqu'il a terminé */
