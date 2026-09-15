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

void ecritures_detruire(Ecriture *ecriture) {
    if (!ecriture) return;
    free(ecriture->lignes);
    ecriture->lignes = NULL;
    ecriture->nombre_lignes = 0;
}

bool ecritures_ligne_valide(const ligne_journal *ligne) {
    if (!ligne || ligne->compte_id == INVALID_ID) return false;
    if (ligne->Debit < 0 || ligne->credit < 0) return false;
    bool a_debit = ligne->Debit > 0;
    bool a_credit = ligne->credit > 0;
    return a_debit != a_credit;  
}

bool ecritures_ligne_valide(const ligne_journal *ligne) {
    if (!ligne || ligne->compte_id == INVALID_ID) return false;
    if (ligne->Debit < 0 || ligne->credit < 0) return false;
    bool a_debit = ligne->Debit > 0;
    bool a_credit = ligne->credit > 0;
    return a_debit != a_credit; 
}
