#ifndef COMPTES_H
#define COMPTES_H

#include <stdint.h>
#include <stdbool.h>
#include <Src/FinCore.Native/include/types.h>
#include <Src/FinCore.Native/include/errors.h>
#include <monnaie.h>
#include <identifiants.h>

/* Création et validation */
FinCore_Etat comptes_creer(Compte *compte, const char *code, const char *nom, FNC_ClasseCompte classe, FNC_TypeCompte type, FNC_SoldeNormal solde_normal, id_compte parent_id);

bool comptes_valider_code(const char *code);
bool comptes_valider(const Compte *compte);

/* Opérations sur le solde */
FinCore_Etat comptes_debiter(Compte *compte, FNC_Monnaie montant);
FinCore_Etat comptes_crediter(Compte *compte, FNC_Monnaie montant);
FNC_Monnaie comptes_get_solde(const Compte *compte);

