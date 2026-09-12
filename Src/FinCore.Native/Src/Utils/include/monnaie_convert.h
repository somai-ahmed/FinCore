#ifndef MONNAIE_CONVERT_H
#define MONNAIE_CONVERT_H

#include <stdbool.h>
#include "types.h"
#include "monnaie.h"

/* Echelle fixe utilisee par le type Monnaie du moteur comptable :
 * 3 decimales (millimes), coherent avec Monnaie_Unite = 1000. */
#define MONNAIE_FIXE_ECHELLE 3

/* Convertit une Monnaie (echelle fixe 3) vers une monnaie a echelle
 * variable. Ne peut pas echouer. */
monnaie monnaie_depuis_fixe(Monnaie m);

/* Convertit une monnaie a echelle variable vers une Monnaie (echelle
 * fixe 3). Si l'echelle source est plus fine que 3, le montant est
 * arrondi (perte de precision). "success" indique un depassement de
 * capacite sur int64_t (false), sinon true. */
Monnaie monnaie_vers_fixe(monnaie m, bool *success); /*inverse du monnaie_depuis_fixe accompagne par un variable du success pour confirmer la validation du convert */

#endif
