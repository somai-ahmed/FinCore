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


#endif
