#include <Src/FinCore.Native/Src/Utils/include/dates.h>

bool date_valide(DATE d){
    if(!d) return FALSE;
    if (d.jour < 1 || d.mois < 1 || d.mois > 12) return false;

    /* mois à 31 jours */ 
    if (d.jour > 31 && (d.mois == 1 || d.mois == 3 || d.mois == 5 || d.mois == 7 || d.mois == 8 || d.mois == 10 || d.mois == 12))
        return false;

    /* mois à 30 jours */
    if (d.jour > 30 && (d.mois == 4 || d.mois == 6 || d.mois == 9 || d.mois == 11))
        return false;
