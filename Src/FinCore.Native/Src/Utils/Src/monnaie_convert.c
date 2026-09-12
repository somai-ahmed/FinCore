#include <Src/FinCore.Native/Src/Utils/include/monnaie_convert.h>

#include <stdint.h>

/* ajouter cette fonction sous le caractere static car elle est dedie juste a faciliter le fonctionnement du cette fichier */
static int64_t puissance_de_dix(int n) {
    int64_t resultat = 1;
    int i;
    for (i = 0; i < n; i++) {
        resultat *= 10;
    }
    return resultat;
}

monnaie monnaie_depuis_fixe(Monnaie m){
    return monnaie_depuis_unites_mineures(m, MONNAIE_FIXE_ECHELLE); /* l"appelle du la fonction du le moteur de la monnaie du base <monnaie.h> */
}
