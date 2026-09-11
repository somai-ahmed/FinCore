#ifndef COMPTES_H
#define COMPTES_H

#include <stdint.h>
#include <stdbool.h>
#include <Src/FinCore.Native/include/types.h>
#include <Src/FinCore.Native/include/errors.h>
#include <monnaie.h>
#include <identifiants.h>

/* Création et validation */
Etat comptes_creer(Compte *compte, const char *code, const char *nom, FNC_ClasseCompte classe, FNC_TypeCompte type, FNC_SoldeNormal solde_normal, id_compte parent_id);

bool comptes_valider_code(const char *code);
bool comptes_valider(const Compte *compte);

/* Opérations sur le solde */
Etat comptes_debiter(Compte *compte, FNC_Monnaie montant);
Etat comptes_crediter(Compte *compte, FNC_Monnaie montant);
Monnaie comptes_avoir_solde(const Compte *compte);

/* Hiérarchie */
bool comptes_a_parent(const Compte *compte);
bool comptes_est_enfant_de(const Compte *compte, id_compte parent_id);

/* Activation */
void comptes_activer(Compte *compte);
void comptes_desactiver(Compte *compte);
