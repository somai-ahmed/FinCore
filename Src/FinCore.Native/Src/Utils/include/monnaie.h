/* ce fichier va contenir tous les propreites concernant les monnaies du systeme financier */
#ifndef MONNAIE_H
#define MONNAIE_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

typedef struct {
    int64_t unites_min ;
    int8_t echelle
}monnaie;

/* creation un montant nul a un echelle specefique */
monnaie initiation_monnaie(int8_t echelle);

/* Cree une monnaie a partir de sa valeur en unites mineures
 * Exemple : monnaie_depuis_unites_mineures(12345, 3)    = 12.345 */
Monnaie monnaie_depuis_unites_mineures( int64_t unites_mineures, int8_t echelle);
#endif
