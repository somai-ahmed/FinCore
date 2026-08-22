/*
 * FinCore Accounting Engine
 * Data Contract Header - FinCore_AE.h
 *
 * Pure C data structures, enums, and constants.
 * No function declarations. No C++ guards. No export macros.
 *
 * Used by:
 *   - C core engine implementation
 *   - VB.NET P/Invoke marshaling layer
 *   - Documentation / interop contract
 */


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
