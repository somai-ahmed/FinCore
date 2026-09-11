#ifndef COMPTES_H
#define COMPTES_H

#include <stdint.h>
#include <stdbool.h>
#include <Src/FinCore.Native/include/FinCore_types.h>
#include <Src/FinCore.Native/include/FinCore_errors.h>
#include <monnaie.h>
#include <identifiants.h>

/* Création et validation */
FinCore_Etat comptes_creer(Compte *compte, const char *code, const char *nom, FNC_ClasseCompte classe, FNC_TypeCompte type, FNC_SoldeNormal solde_normal, id_compte parent_id);

bool comptes_valider_code(const char *code);
bool comptes_valider(const Compte *compte);

