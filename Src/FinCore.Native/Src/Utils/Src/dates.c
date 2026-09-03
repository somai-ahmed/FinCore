#include <Src/FinCore.Native/Src/Utils/include/dates.h>

bool date_valide(DATE d){
    if(!d) return FALSE;
    if (d.jour < 1 || d.mois < 1 || d.mois > 12) return false;
    
