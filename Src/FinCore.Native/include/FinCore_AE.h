#ifndef FinCore_AE_H
#define FinCore_AE_H

#include "FinCore_types.h"
#include "FinCore_errors.h"
#include "FinCore_detection.h"


#ifndef FINCORE_AE_H
#define FINCORE_AE_H

/*=======definition des constants========*/
#define FINC_MAX_LABEL_LEN               256
#define FINC_MAX_ACCOUNT_CODE_LEN        16
#define FINC_MAX_CURRENCY_LEN            4
#define FINC_MAX_ENTRY_ID_LEN            32
#define FINC_MAX_PERIOD_ID_LEN           16
#define FINC_MAX_USER_ID_LEN             32
#define FINC_MAX_REFERENCE_LEN           64
#define FINC_MAX_SECTION_LEN             64
#define FINC_MAX_DATE_LEN                11
#define FINC_MAX_DATETIME_LEN            20
#define FINC_MAX_SESSIONS                16
#define FINC_MAX_ACCOUNTS                1000
#define FINC_MAX_PERIODS                 50
#define FINC_MAX_JOURNAL_LINES           100000
#define FINC_MAX_ENTRIES                 50000
#define FINC_MAX_DETECTION_HITS          10000
#define FINC_MAX_REPORT_LINES            50000
#define FINC_BENFORD_MIN_SAMPLE          300

/*=======les enumerations========*/
typedef enum {
    FINC_OK = 0,
    FINC_ERR_INVALID_INPUT = -1,
    FINC_ERR_OUT_OF_MEMORY = -2,
    FINC_ERR_ACCOUNT_NOT_FOUND = -3,
    FINC_ERR_UNBALANCED_ENTRY = -4,
    FINC_ERR_PERIOD_CLOSED = -5,
    FINC_ERR_DUPLICATE_KEY = -6,
    FINC_ERR_FILE_IO = -7,
    FINC_ERR_DETECTION = -8,
    FINC_ERR_SESSION_NOT_FOUND = -9,
    FINC_ERR_PERIOD_NOT_FOUND = -10,
    FINC_ERR_ENTRY_NOT_FOUND = -11,
    FINC_ERR_ACCOUNT_IN_USE = -12,
    FINC_ERR_ZERO_AMOUNT = -13,
    FINC_ERR_INVALID_DATE = -14,
    FINC_ERR_INSUFFICIENT_DATA = -15
} FINC_Status;

typedef enum {
    FINC_ACTIF = 1,
    FINC_PASSIF = 2,
    FINC_CHARGE = 3,
    FINC_PRODUIT = 4
} FINC_AccountType;

typedef enum {
    FINC_CAT_BALANCE = 0,
    FINC_CAT_BILAN = 1,
    FINC_CAT_RESULTAT = 2
} FINC_AccountCategory;

typedef enum {
    FINC_PERIOD_OPEN = 0,
    FINC_PERIOD_CLOSED = 1,
    FINC_PERIOD_LOCKED = 2
} FINC_PeriodStatus;

typedef enum {
    FINC_DET_BENFORD = 0x01,
    FINC_DET_DUPLICATE = 0x02,
    FINC_DET_ROUND_NUMBER = 0x04,
    FINC_DET_OUTLIER = 0x08,
    FINC_DET_ALL = 0x0F
} FINC_DetectionFlags; /*ouvre documentation fichier pour l'explication du binaire*/

/*=========STRUCTURE DES DONNEES=========*/
typedef struct {
    char    code[FINC_MAX_ACCOUNT_CODE_LEN];
    char    label[FINC_MAX_LABEL_LEN];
    int     type;
    int     category;
    char    parent[FINC_MAX_ACCOUNT_CODE_LEN];
    double  opening_balance;
    int     is_active;
} FINC_Account;

typedef struct {
    char    entry_id[FINC_MAX_ENTRY_ID_LEN];
    char    date[FINC_MAX_DATE_LEN];
    char    account_code[FINC_MAX_ACCOUNT_CODE_LEN];
    char    label[FINC_MAX_LABEL_LEN];
    char    reference[FINC_MAX_REFERENCE_LEN];
    double  debit;
    double  credit;
    char    currency[FINC_MAX_CURRENCY_LEN];
    double  exchange_rate;
    char    period_id[FINC_MAX_PERIOD_ID_LEN];
    char    user_id[FINC_MAX_USER_ID_LEN];
    char    created_at[FINC_MAX_DATETIME_LEN];
} FINC_JournalLine;
