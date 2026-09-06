#include <Src/FinCore.Native/Src/Utils/include/monnaie.h>
#include <ctype.h>

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

monnaie monnaie_depuis_chaine(const char *chaine, int8_t echelle, bool success){
    monnaie m = initiation_monnaie(echelle);
    if (success) success = false ;
    if (!chaine || echelle < 0) {return m ; }

    const char *p = chaine;

    /* 1. signe optionnel */
    int negatif = 0;
    if (*p == '+' || *p == '-') {
        negatif = (*p == '-'); /* negatif recoit 1 si negatif (variable booleene) */
        p++;
    }

    /* il faut au moins un chiffre après le signe */
    if (!isdigit((unsigned char)*p)) {
        success = false;
        return m;
    }

    /* partie numerique */
    int64_t valeur = 0;
    while (isdigit((unsigned char)*p)) {
        int chiffre = *p - '0';

        if (valeur > (INT64_MAX - chiffre) / 10) {
            success = false;
            return m;  // overflow -> echec
            
        }

        valeur = valeur * 10 + chiffre;
        p++;
    }

    /* 3. partie fractionnaire (optionnelle) */
    int8_t partie_decimale = 0;
    if (*p == '.' || *p == ',') {
        p++;

        while (isdigit((unsigned char)*p)) {
            if (partie_decimale >= echelle) {
                return m;  // trop de decimales pour l'echelle -> rejet
            }

            int chiffre = *p - '0';

            if (valeur > (INT64_MAX - chiffre) / 10) {
                return m;  // overflow
            }

            valeur = valeur * 10 + chiffre;
            partie_decimale++;
            p++;
        }
    }
    /* 4. il ne doit rien rester d'invalide en fin de chaine */
    if (*p != '\0') {
        return m;
    }

    /* 5. completer avec des zeros si moins de decimales que l'echelle */
    while (decimales_lues < echelle) {
        if (valeur > INT64_MAX / 10) {
            return m;  // overflow (cas extreme)
        }
        valeur *= 10;
        decimales_lues++;
    }

    /* 6. appliquer le signe et valider */
    m.valeur_mineure = negatif ? -valeur : valeur;
    m.echelle = echelle;

    if (success) *success = true;
    return m;
}
