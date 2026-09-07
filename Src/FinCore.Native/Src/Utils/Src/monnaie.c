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
    m.unites_min = negatif ? -valeur : valeur;
    m.echelle = echelle;

    if (success) *success = true;
    return m;
}

monnaie ajouter_monnaie(monnaie m1 ,monnaie m2 , bool success){
    monnaie resultat = initiation_monnaie(m1.echelle);
    if (success) *success = false;  // echec

    /* regle d'or : on n'additionne que des montants de meme echelle */
    if (m1.echelle != m2.echelle) {
        return resultat;
    }
    /* verification d'overflow AVANT l'addition, pas apres */
    if ((m2.unites_min > 0 && m1.unites_min > INT64_MAX - m2.unites_min) || (m2.unites_min < 0 && m1.unites_min < INT64_MIN - m2.unites_min)) {
        return resultat;
    }
    
    resultat.unites_min = m1.unites_min + m2.unites_min;
    resultat.echelle = m1.echelle;

    if (success) *success = true;
    return resultat;
}

monnaie soustraire_monnaie(monnaie m1, monnaie m2, bool *success) {
    
    monnaie resultat = initiation_monnaie(m1.echelle);

    if (success) *success = false;  // echec 

    /* meme regle que l'addition : echelles identiques obligatoires */
    if (m1.echelle != m2.echelle) {
        return resultat;
    }

    /* verification d'overflow/underflow AVANT la soustraction :
       m1 - m2 deborde si :
       - m2 est positif et m1 est deja trop proche de INT64_MIN
       - m2 est negatif et m1 est deja trop proche de INT64_MAX */
    if ((m2.unites_min > 0 && m1.unites_min < INT64_MIN + m2.unites_min) ||
        (m2.unites_min < 0 && m1.unites_min > INT64_MAX + m2.unites_min)) {
        return resultat;
    }

    resultat.unites_min = m1.unites_min - m2.unites_min;
    resultat.echelle = m1.echelle;

    if (success) *success = true;
    return resultat;
}

/* ============================================================
 * Multiplication par points de base
 * ============================================================ */
monnaie monnaie_multiplier_points_de_base(monnaie m, int32_t points_de_base, bool *success) {

    monnaie resultat = initiation_monnaie(m.echelle);

    if (success) *success = false; // echec par defaut

    if (m.echelle < 0) {
        return resultat;
    }

    /* on calcule en int64 : unites_min * points_de_base, puis on divise par 10000
       (10000 points de base = 100.00%) */

    int64_t valeur = m.unites_min;
    int64_t pdb = (int64_t)points_de_base;

    /* verification d'overflow AVANT la multiplication */
    if (valeur != 0) {
        if (pdb > 0) {
            if (valeur > 0 && valeur > INT64_MAX / pdb) {
                return resultat;
            }
            if (valeur < 0 && valeur < INT64_MIN / pdb) {
                return resultat;
            }
        } else if (pdb < 0) {
            if (valeur > 0 && valeur > INT64_MIN / pdb) {
                return resultat;
            }
            if (valeur < 0 && valeur < INT64_MAX / pdb) {
                return resultat;
            }
        }
    }

    int64_t produit = valeur * pdb;

    /* division entiere par 10000 (arrondi vers zero) */
    int64_t resultat_valeur = produit / 10000;

    resultat.unites_min = resultat_valeur;
    resultat.echelle = m.echelle;

    if (success) *success = true;
    return resultat;
}

/*--------------------------
        COMPARISON           
---------------------------*/

/* l'output de cette fonction est entier 
    on va tester et retourner 3 majeur nombre
    la fonction retourne :
        --> 0 :: si un erreur
        --> -1 :: si le 2eme montant superieur au premier
        --> 1 :: si le 1er montant superieur au premier
*/
int comparer_monnaie(monnaie m1,monnaie m2){
    if (m1.echelle != m2.echelle) {
        return 0;
    }

    if (m1.valeur_mineure < m2.valeur_mineure) {
        return -1;
    }
    if (m1.valeur_mineure > m2.valeur_mineure) {
        return 1;
    }
    return 0;
}

/* cette fonction retourne une variable booleenne cad
    du "VRAI" ou "FAUX" 
    si le montant est negatif :: la fonction retourne VRAI et inversement
    */
bool monnaie_est_negative(monnaie m) {
    return m.valeur_mineure < 0;
}
/* cette fonction retourne une variable booleenne cad
    du "VRAI" ou "FAUX" 
    si le montant est NULLE :: la fonction retourne VRAI et sinon FAUX
    */
bool monnaie_est_nulle(monnaie m) {
    return m.valeur_mineure == 0;
}

/*----------------------------
        FOMRATAGE
-----------------------------*/
bool monnaie_vers_chaine(monnaie m, char *tampon, size_t taille_tampon) {
    if (!tampon || taille_tampon == 0 || m.echelle < 0) {
        return false;
    }
    int64_t valeur = m.valeur_mineure;
    bool negatif = valeur < 0;

    uint64_t valeur_abs = negatif ? (uint64_t)(-(valeur + 1)) + 1 : (uint64_t)valeur;

    /* on construit la chaine des chiffres a l'envers dans un tampon local */
    char chiffres[32];
    int i = 0;

    if (valeur_abs == 0) {
        chiffres[i++] = '0';
    } else {
        while (valeur_abs > 0 && n < (int)sizeof(chiffres)) {
            chiffres[i++] = (char)('0' + (valeur_abs % 10));
            valeur_abs /= 10;
        }
    }
    /* completer avec des zeros a gauche si moins de chiffres que l'echelle + 1
       (il faut au moins un chiffre avant la virgule) */
    while (n < m.echelle + 1 && n < (int)sizeof(chiffres)) {
        chiffres[n++] = '0';
    }

    /* construction de la chaine finale */
    size_t pos = 0;

    if (negatif) {
        if (pos + 1 >= taille_tampon) return false;
        tampon[pos++] = '-';
    }

    int partie_entiere_len = n - m.echelle;

    /* partie entiere (les chiffres sont dans chiffres[] a l'envers) */
    for (int i = 0; i < partie_entiere_len; i++) {
        if (pos + 1 >= taille_tampon) return false;
        tampon[pos++] = chiffres[n - 1 - i];
    }

    /* partie decimale */
    if (m.echelle > 0) {
        if (pos + 1 >= taille_tampon) return false;
        tampon[pos++] = '.';

        for (int i = 0; i < m.echelle; i++) {
            if (pos + 1 >= taille_tampon) return false;
            tampon[pos++] = chiffres[m.echelle - 1 - i];
        }
    }

    if (pos + 1 > taille_tampon) return false;
    tampon[pos] = '\0';

    return true ;
}
