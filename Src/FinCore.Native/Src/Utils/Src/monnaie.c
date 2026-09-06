#include <Src/FinCore.Native/Src/Utils/include/monnaie.h>

/* creation un montant nul a un echelle specefique */
monnaie initiation_monnaie(int8_t echelle){
    monnaie m;
    m.unites_min  = 0;
    m.echelle = echelle;
    return m;
}
