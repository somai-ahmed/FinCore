#include <Src/FinCore.Native/Src/Utils/include/ecritures.h>

#include <Src/FinCore.Native/Src/Utils/include/dates.h>
#include <string.h>
#include <stdlib.h>

Etat ecritures_creer(Ecriture *ecriture, idperiodefiscale periode_id, DATE date, const char *reference, const char *description) {
  
    if (!ecriture || !reference || !description) return ERR_POINTEUR_NULLE;
    if (!date_valide(date)) return ERR_DATE_JOURNAL_INVALIDE;
    if (description[0] == '\0') return ERR_LIBELLE_JOURNAL_MANQUANT;

    memset(ecriture, 0, sizeof(Ecriture));
    ecriture->id = 0; 
    ecriture->periode_id = periode_id;
    ecriture->date = date;
    strncpy(ecriture->reference, reference, sizeof(ecriture->reference) - 1);
    strncpy(ecriture->description, description, sizeof(ecriture->description) - 1);

    return ETAT_OK;
}
