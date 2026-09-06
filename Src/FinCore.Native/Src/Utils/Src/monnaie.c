#include <Src/FinCore.Native/Src/Utils/include/monnaie.h>

/* creation un montant nul a un echelle specefique */
monnaie initiation_monnaie(int8_t echelle){
    monnaie m;
    m.unites_min  = 0;
    m.echelle = echelle;
    return m;
}

monnaie monnaie_depuis_unites_mineures( int64_t unites_min, int8_t echelle){
    monnaie m = initiation_monnaie(echelle);
    m.unites_min = unites_min ;
    return m;
}

    
