#include <Src/FinCore.Native/Src/Utils/include/dates.h>


bool est_annee_bissextile(int32_t annee){
    return (annee % 4 == 0 && annee % 100 != 0) || (annee % 400 == 0);
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
    if (est_annee_bissextile(d.annee)) {
            if (d.jour > FEVRIER_JOURS_BISSEXTILE) return false;
        }
    else {
            if (d.jour > FEVRIER_JOURS_NORMAL) return false;
        }
    
    return true;
}
