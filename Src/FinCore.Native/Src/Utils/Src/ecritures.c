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


Etat ecritures_ajouter_ligne(Ecriture *ecriture, id_compte compte_id, Monnaie debit, Monnaie credit, const char *libelle) {
    if (!ecriture || !libelle) return ERR_POINTEUR_NULLE;
    if (ecriture->est_validee) return ERR_JOURNAL_DEJA_COMPTABILISE;
    if (compte_id == INVALID_ID) return ERR_COMPTE_DEBIT_INVALIDE;
    if (debit < 0 || credit < 0) return ERR_JOURNAL_MONTANT_NEGATIF;
    if (debit > 0 && credit > 0) return ERR_JOURNAL_DEBIT_CREDIT_SIMULTANES;
    if (debit == 0 && credit == 0) return ERR_JOURNAL_SANS_MONTANT;

    ligne_journal *nouvelles = realloc(ecriture->lignes, (ecriture->nombre_lignes + 1) * sizeof(ligne_journal));
    if (!nouvelles) return ERR_SORTIE_DU_MEMOIRE;
    ecriture->lignes = nouvelles;

    ligne_journal *ligne = &ecriture->lignes[ecriture->nombre_lignes];
    ligne->id = (id_ligne)(ecriture->nombre_lignes + 1);
    ligne->compte_id = compte_id;
    ligne->Debit = debit;
    ligne->credit = credit;
    strncpy(ligne->libelle, libelle, sizeof(ligne->libelle) - 1);
    ligne->libelle[sizeof(ligne->libelle) - 1] = '\0';

    ecriture->nombre_lignes++;
    return ETAT_OK;
}

size_t ecritures_nombre_lignes(const Ecriture *ecriture) {
    return ecriture ? ecriture->nombre_lignes : 0;
}

bool ecritures_avoir_ligne(const Ecriture *ecriture, size_t index, ligne_journal *ligne_out) {
    if (!ecriture || !ligne_out || index >= ecriture->nombre_lignes) return false;
    *ligne_out = ecriture->lignes[index];
    return true;
}

Monnaie ecritures_total_debit(const Ecriture *ecriture) {
    Monnaie total = 0;
    if (!ecriture) return total;
    for (size_t i = 0; i < ecriture->nombre_lignes; i++) total += ecriture->lignes[i].Debit;
    return total;
}

Monnaie ecritures_total_credit(const Ecriture *ecriture) {
    Monnaie total = 0;
    if (!ecriture) return total;
    for (size_t i = 0; i < ecriture->nombre_lignes; i++) total += ecriture->lignes[i].credit;
    return total;
}
