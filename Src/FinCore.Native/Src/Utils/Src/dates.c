#include <Src/FinCore.Native/Src/Utils/include/dates.h>


bool est_annee_bissextile(int32_t annee){
    return (annee % 4 == 0 && annee % 100 != 0) || (annee % 400 == 0);
}

int32_t jours_dans_mois(int32_t mois,int32_t annee){
    switch(mois)
        case 1:
        case 3:
        case 5:
        case 7:
        case 8:
        case 10:
        case 12:
            return 31; 

        case 4:
        case 6:
        case 9:
        case 11:
            return 30;

        case 2 : 
            return est_annee_bissextile(annee) ? FEVRIER_JOURS_BISSEXTILE : FEVRIER_JOURS_NORMAL;

        default :
            return 0 ;
}        

bool date_valide(DATE d){
    if (d.jour < 1 || d.mois < 1 || d.mois > 12) return false;

    /* mois à 31 jours */ 
    if (d.jour > 31 && (d.mois == 1 || d.mois == 3 || d.mois == 5 || d.mois == 7 || d.mois == 8 || d.mois == 10 || d.mois == 12))
        return false;

    /* mois à 30 jours */
    if (d.jour > 30 && (d.mois == 4 || d.mois == 6 || d.mois == 9 || d.mois == 11))
        return false;

    /*fevrier : le cas particulier*/
    /*l'apppel du la fonction du l'annee bissextile*/
    if (d.mois == 2) {
        int max_jour_en_fevrier = est_annee_bissextile(d.annee)? FEVRIER_JOURS_BISSEXTILE : FEVRIER_JOURS_NORMAL;
        if (d.jour > max_jour_en_fevrier)
            return false;
    }
    return true ; /*date valide et propre*/
}

int comparer_dates(DATE d1, DATE d2){
    /* le type d'output du la fonction comparer_dates est 'integer' : 
        c'est le cas ou ::    la fonction retourne -2 , si la date est invalide 
                              la fonction retourne -1 si la 2eme date est superieure que la premiere et 1 en inverse
                              sinon la foction retourne 0 :: si les deux dates sont identiques*/
    
    if (!date_valide(d1) || !date_valide(d2)) return -2;

    if (d1.annee != d2.annee) return (d1.annee < d2.annee) ? -1 : 1;

    if (d1.mois != d2.mois) return (d1.mois < d2.mois) ? -1 : 1;

    if (d1.jour != d2.jour) return (d1.jour < d2.jour) ? -1 : 1;

    return 0 ; /*d1 et d2 sont identiques*/
}

bool date_inclus_dans_periode (DATE d , DATE debut_periode , DATE fin_periode){
    /* la fonction est du type d'output 'BOOLEEN' , c'est a dire elle retourne 
        vrai ou faux selon la condition 

    * le logique du cette fonction va etre comme ca
            -> analyse de la date : appliquer tous les verification
            -> comparaison
            (d'autre maniere :: l'usage des fonctions precedents */
    if (!date_valide(d) || !date_valide(debut_periode) || !date_valide(fin_periode) return false;

    if ( comparer_dates(debut_periode , fin_periode) == -1) return false;

    if(comparer_dates(debut_periode,fin_periode)==0 && !comparer_dates(d,debut_periode)) return false ;

    return true;
}
    
DATE ajouter_jours_au_date(DATE d,int32_t jours){
    if(!date_valide(d)) return d;

    if (jours<0) return d;

    while(jours>0){
        int32_t jours_mois = jours_dans_mois(d.mois,d.annee);
        if(d.jour+jours <= jours_mois){
            d.jour+=jours;
            jours = 0; /*bloquage du boucle while*/
        }
        else{
            jours = jours_mois - d.jour + 1;
            d.jour = 1 ;

            if(d.mois == 12){
                d.annee++;
                d.mois = 1;
            }
            else{
                d.mois++ ;
            }
        }
    }
    return d;
    
}

DATE difference_date(DATE d1 ,DATE d2){
    if (!date_valide(d1) || !date_valide(d2))
        return DATE_ERREUR;
    

