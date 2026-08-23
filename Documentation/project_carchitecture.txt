# FinCore Accounting & Fraud Detection Engine
## Project Explication Book — v1.0

---

## 1. Executive Summary

**Project Name:** FinCore Accounting Engine (FinCore-AE)  
**Type:** Desktop accounting system with embedded fraud detection  
**Primary Language:** C (core engine, Windows DLL)  
**GUI Layer:** VB.NET / Windows Forms  
**Excel Handling:** ClosedXML (no Excel Interop dependency)  
**Persistence:** Flat-file (JSON/CSV hybrid) — MVP only  
**Target User:** SME accountant / auditor performing year-end closing and anomaly detection  
**Demo Value:** One project demonstrating systems programming, cross-language interop, desktop GUI, file-format engineering, applied statistics, and accounting domain knowledge.

---

## 2. System Architecture

### 2.1 Layer Diagram

```
+-------------------------------------------------------------+
|                    VB.NET / Windows Forms                    |
|  +-------------+  +-------------+  +---------------------+  |
|  |  Forms UI   |  |  P/Invoke   |  |   ClosedXML         |  |
|  |  (Screens)  |  |  (Marshaling|  |   (Excel I/O)       |  |
|  |             |  |   Layer)    |  |                     |  |
|  +------+------+  +------+------+  +----------+----------+  |
|         |                |                     |             |
|         +----------------+---------------------+             |
|                          |                                   |
|                    Application Controller                    |
|              (state machine, validation, routing)            |
+--------------------------+-----------------------------------+
                           |
                    P/Invoke Boundary
                           |
+--------------------------+-----------------------------------+
|              C Core Engine (Windows DLL — FinCore_ae.dll)         |
|  +-------------+  +-------------+  +---------------------+  |
|  |  Journal    |  |  Reporting  |  |   Detection Engine  |  |
|  |  Engine     |  |  (GL/Bal/Bil|  |   (Benford + Rules) |  |
|  |             |  |             |  |                     |  |
|  +-------------+  +-------------+  +---------------------+  |
|  +-------------+  +-------------+  +---------------------+  |
|  |  Account    |  |  Period     |  |   Flat-File         |  |
|  |  Chart      |  |  Manager    |  |   Persistence       |  |
|  |  (Plan      |  |             |  |   (JSON/CSV)        |  |
|  |  Comptable) |  |             |  |                     |  |
|  +-------------+  +-------------+  +---------------------+  |
+-------------------------------------------------------------+
```

### 2.2 Design Principles

| Principle | Rationale |
|-----------|-----------|
| **C core = pure math + logic** | No file-format, no UI, no networking. Testable in isolation. Portable if needed. |
| **VB.NET = orchestration + presentation** | No business logic. Calls C DLL, formats output, drives ClosedXML. |
| **ClosedXML over Interop** | No Excel installation required. Faster. Headless-friendly. Strong for CI/demo. |
| **Flat-file persistence** | Zero dependency. Human-readable. Easy to version-control test data. |
| **Struct-based API** | Blittable types only. No memory allocation surprises across the P/Invoke boundary. |

---

## 3. C Core Engine — Data Structures

### 3.1 Fundamental Types

```c
// FinCore_ae_types.h

#define FinCore_MAX_LABEL_LEN       256
#define FinCore_MAX_ACCOUNT_CODE_LEN 16
#define FinCore_MAX_CURRENCY_LEN     4
#define FinCore_MAX_ENTRIES_PER_JOURNAL 10000
#define FinCore_MAX_ACCOUNTS         1000
#define FinCore_MAX_PERIODS          50

typedef enum {
    FinCore_OK = 0,
    FinCore_ERR_INVALID_INPUT = -1,
    FinCore_ERR_OUT_OF_MEMORY = -2,
    FinCore_ERR_ACCOUNT_NOT_FOUND = -3,
    FinCore_ERR_UNBALANCED_ENTRY = -4,
    FinCore_ERR_PERIOD_CLOSED = -5,
    FinCore_ERR_DUPLICATE_KEY = -6,
    FinCore_ERR_FILE_IO = -7,
    FinCore_ERR_DETECTION = -8
} FinCore_Status;

typedef enum {
    FinCore_DET_BENFORD = 0x01,
    FinCore_DET_DUPLICATE = 0x02,
    FinCore_DET_ROUND_NUMBER = 0x04,
    FinCore_DET_OUTLIER = 0x08,
    FinCore_DET_ALL = 0x0F
} FinCore_DetectionFlags;
```

### 3.2 Account Structure (Plan Comptable)

```c
typedef struct {
    char     code[FinCore_MAX_ACCOUNT_CODE_LEN];   // e.g., "512", "701", "120"
    char     label[FinCore_MAX_LABEL_LEN];         // e.g., "Banques", "Ventes France"
    int      type;                             // 1=Actif, 2=Passif, 3=Charge, 4=Produit
    int      category;                         // 0=Balance, 1=Bilan, 2=Compte de resultat
    char     parent[FinCore_MAX_ACCOUNT_CODE_LEN]; // Hierarchical parent code (empty = root)
    double   opening_balance;                  // Solde d'ouverture
    int      is_active;                        // 0=disabled, 1=active
} FinCore_Account;
```

**Account Type Mapping (French PCG-aligned):**

| Type ID | Label | Normal Balance | Bilan / CR |
|---------|-------|----------------|------------|
| 1 | Actif | Debit | Bilan |
| 2 | Passif | Credit | Bilan |
| 3 | Charge | Debit | Compte de Resultat |
| 4 | Produit | Credit | Compte de Resultat |

**Standard Account Ranges:**

| Class | Range | Nature |
|-------|-------|--------|
| 1 | 100-199 | Comptes de capitaux |
| 2 | 200-299 | Comptes d'actif immobilise |
| 3 | 300-399 | Comptes de stocks |
| 4 | 400-499 | Comptes de tiers |
| 5 | 500-599 | Comptes de tresorerie |
| 6 | 600-699 | Comptes de charges |
| 7 | 700-799 | Comptes de produits |

### 3.3 Journal Entry Structure

```c
typedef struct {
    char     entry_id[32];                     // UUID or sequential "E-2024-00001"
    char     date[11];                         // ISO-8601: "2024-03-15"
    char     account_code[FinCore_MAX_ACCOUNT_CODE_LEN];
    char     label[FinCore_MAX_LABEL_LEN];         // Libelle de l'ecriture
    char     reference[64];                    // Piece justificative
    double   debit;                            // Montant au debit
    double   credit;                           // Montant au credit
    char     currency[FinCore_MAX_CURRENCY_LEN];   // "EUR", "USD"
    double   exchange_rate;                    // 1.0 for base currency
    char     period_id[16];                    // Links to FinCore_Period
    char     user_id[32];                      // Audit trail
    char     created_at[20];                   // ISO-8601 datetime
} FinCore_JournalLine;
```

**Journal Entry Rules (enforced by C engine):**
- Every entry must have at least 2 lines (double-entry principle)
- Sum(debit) == Sum(credit) for a given entry_id (balanced)
- date must fall within an open period
- account_code must exist in the chart and be active
- Either debit or credit must be zero on a single line (never both, never neither)
- Amount precision: 2 decimal places (rounded to nearest cent)

### 3.4 Period Structure

```c
typedef struct {
    char     period_id[16];                    // e.g., "2024-01", "2024-FY"
    char     label[FinCore_MAX_LABEL_LEN];         // "Janvier 2024", "Exercice 2024"
    char     start_date[11];                   // "2024-01-01"
    char     end_date[11];                     // "2024-01-31"
    int      status;                           // 0=open, 1=closed, 2=locked
    char     closed_by[32];                    // User who closed it
    char     closed_at[20];                    // ISO-8601 datetime
} FinCore_Period;
```

### 3.5 Grand Livre Line (Report Output)

```c
typedef struct {
    char     date[11];
    char     entry_id[32];
    char     reference[64];
    char     label[FinCore_MAX_LABEL_LEN];
    double   debit;
    double   credit;
    double   running_balance;                  // Cumul after this line
} FinCore_GLLine;
```

### 3.6 Balance Line (Report Output)

```c
typedef struct {
    char     account_code[FinCore_MAX_ACCOUNT_CODE_LEN];
    char     label[FinCore_MAX_LABEL_LEN];
    double   opening_balance;
    double   total_debit;
    double   total_credit;
    double   closing_balance;                  // opening + debit - credit (or per account type)
    int      account_type;
} FinCore_BalanceLine;
```

### 3.7 Bilan Line (Report Output)

```c
typedef struct {
    char     section[64];                      // "ACTIF", "PASSIF", "CAPITAUX PROPRES"
    char     account_code[FinCore_MAX_ACCOUNT_CODE_LEN];
    char     label[FinCore_MAX_LABEL_LEN];
    double   gross_amount;                     // Brut
    double   depreciation;                     // Amortissements / Depreciations
    double   net_amount;                       // Net
    int      display_order;                    // Sort order for presentation
    int      is_total_row;                     // 1 = subtotal / total line
    int      indent_level;                     // 0=section, 1=category, 2=account
} FinCore_BilanLine;
```

### 3.8 Detection Result Structure

```c
typedef struct {
    char     entry_id[32];
    char     account_code[FinCore_MAX_ACCOUNT_CODE_LEN];
    char     date[11];
    double   amount;
    int      flag;                             // Which rule triggered (bitmask)
    double   score;                            // 0.0-1.0 severity / confidence
    char     description[FinCore_MAX_LABEL_LEN];   // Human-readable explanation
    char     recommendation[FinCore_MAX_LABEL_LEN];// Suggested action
} FinCore_DetectionHit;

typedef struct {
    int                hit_count;
    FinCore_DetectionHit*  hits;                   // Allocated by C, freed by caller
    double             benford_chi_square;     // x2 statistic for digit-1 test
    double             benford_p_value;        // Statistical significance
    int                benford_sample_size;    // Number of transactions tested
} FinCore_DetectionReport;
```

### 3.9 Benford Distribution (for charting)

```c
typedef struct {
    int      digit;                            // 1-9 (leading digit)
    double   expected_frequency;               // log10(1 + 1/digit)
    double   actual_frequency;                 // Observed in dataset
    int      actual_count;                     // Raw count
    double   deviation;                        // actual - expected
} FinCore_BenfordDigit;
```


---

## 4. C Core Engine — DLL Export API

### 4.1 Lifecycle & Context

```c
// FinCore_ae.h

#ifdef FinCore_AE_EXPORTS
#define FinCore_API __declspec(dllexport)
#else
#define FinCore_API __declspec(dllimport)
#endif

// Creates a new accounting session (in-memory context)
// Returns: opaque handle (void*) or NULL on failure
FinCore_API void* FinCore_CreateSession(const char* company_name,
                                const char* base_currency,
                                const char* fiscal_year_start);

// Destroys session and frees all associated memory
FinCore_API void  FinCore_DestroySession(void* session);

// Persists current session to disk
FinCore_API FinCore_Status FinCore_SaveSession(void* session, const char* filepath);

// Loads session from disk
FinCore_API void* FinCore_LoadSession(const char* filepath);

// Returns last error message as null-terminated string
FinCore_API const char* FinCore_GetLastError(void* session);
```

### 4.2 Account Chart Management

```c
// Adds or updates an account in the chart
FinCore_API FinCore_Status FinCore_SetAccount(void* session, const FinCore_Account* account);

// Retrieves account by code. Returns FinCore_OK or FinCore_ERR_ACCOUNT_NOT_FOUND
FinCore_API FinCore_Status FinCore_GetAccount(void* session,
                                   const char* code,
                                   FinCore_Account* out_account);

// Deletes account (only if balance is zero and no journal lines exist)
FinCore_API FinCore_Status FinCore_DeleteAccount(void* session, const char* code);

// Returns count of accounts
FinCore_API int FinCore_GetAccountCount(void* session);

// Retrieves all accounts. Caller provides array of sufficient size.
FinCore_API FinCore_Status FinCore_GetAllAccounts(void* session,
                                       FinCore_Account* out_accounts,
                                       int max_count,
                                       int* out_actual_count);

// Loads standard French PCG chart (classes 1-7)
FinCore_API FinCore_Status FinCore_LoadStandardChart(void* session);
```

### 4.3 Period Management

```c
FinCore_API FinCore_Status FinCore_CreatePeriod(void* session, const FinCore_Period* period);
FinCore_API FinCore_Status FinCore_GetPeriod(void* session,
                                  const char* period_id,
                                  FinCore_Period* out_period);
FinCore_API FinCore_Status FinCore_ClosePeriod(void* session, const char* period_id);
FinCore_API FinCore_Status FinCore_ReopenPeriod(void* session, const char* period_id);
FinCore_API int        FinCore_GetPeriodCount(void* session);
```

### 4.4 Journal Entry Operations

```c
// Posts a multi-line journal entry.
// The array of lines must balance (sum debit == sum credit).
// All lines share the same entry_id and date.
FinCore_API FinCore_Status FinCore_PostEntry(void* session,
                                  const FinCore_JournalLine* lines,
                                  int line_count);

// Retrieves all lines for a given entry_id
FinCore_API FinCore_Status FinCore_GetEntry(void* session,
                                 const char* entry_id,
                                 FinCore_JournalLine* out_lines,
                                 int max_count,
                                 int* out_actual_count);

// Retrieves all entries in a date range
FinCore_API FinCore_Status FinCore_GetEntriesByDateRange(void* session,
                                              const char* start_date,
                                              const char* end_date,
                                              FinCore_JournalLine* out_lines,
                                              int max_count,
                                              int* out_actual_count);

// Reverses an entry (creates contra-entry with same amounts, opposite signs)
FinCore_API FinCore_Status FinCore_ReverseEntry(void* session,
                                     const char* entry_id,
                                     const char* reversal_date,
                                     char* out_reversal_id);

// Deletes an entry entirely (only if period is open)
FinCore_API FinCore_Status FinCore_DeleteEntry(void* session, const char* entry_id);

// Returns total number of journal lines
FinCore_API int FinCore_GetJournalLineCount(void* session);
```

### 4.5 Reporting Engine

```c
// -- Grand Livre (General Ledger) --
// Generates GL for a single account over a period range
FinCore_API FinCore_Status FinCore_GenerateGL(void* session,
                                   const char* account_code,
                                   const char* start_date,
                                   const char* end_date,
                                   FinCore_GLLine* out_lines,
                                   int max_count,
                                   int* out_actual_count);

// -- Balance (Trial Balance) --
// Generates trial balance for all accounts in a period
FinCore_API FinCore_Status FinCore_GenerateBalance(void* session,
                                        const char* period_id,
                                        FinCore_BalanceLine* out_lines,
                                        int max_count,
                                        int* out_actual_count);

// -- Bilan (Balance Sheet) --
// Generates balance sheet as of a given date
FinCore_API FinCore_Status FinCore_GenerateBilan(void* session,
                                      const char* as_of_date,
                                      FinCore_BilanLine* out_lines,
                                      int max_count,
                                      int* out_actual_count);

// -- Compte de Resultat (P&L) --
// Generates income statement for a period range
FinCore_API FinCore_Status FinCore_GenerateCompteResultat(void* session,
                                               const char* start_date,
                                               const char* end_date,
                                               FinCore_BilanLine* out_lines,
                                               int max_count,
                                               int* out_actual_count);
```

### 4.6 Detection Engine

```c
// Runs detection rules on a dataset
// flags: bitmask of FinCore_DetectionFlags
// dataset: "all", or a specific account_code, or date range "2024-01-01:2024-12-31"
FinCore_API FinCore_Status FinCore_RunDetection(void* session,
                                     int flags,
                                     const char* dataset,
                                     FinCore_DetectionReport* out_report);

// Frees detection report memory allocated by C engine
FinCore_API void FinCore_FreeDetectionReport(FinCore_DetectionReport* report);

// Gets Benford distribution for charting
FinCore_API FinCore_Status FinCore_GetBenfordDistribution(void* session,
                                               const char* dataset,
                                               FinCore_BenfordDigit* out_digits,
                                               int* out_sample_size);

// -- Individual Rule APIs (for granular control) --
FinCore_API FinCore_Status FinCore_CheckBenford(void* session,
                                     const char* dataset,
                                     double* out_chi_square,
                                     double* out_p_value,
                                     int* out_sample_size);

FinCore_API FinCore_Status FinCore_CheckDuplicates(void* session,
                                        const char* dataset,
                                        FinCore_DetectionHit* out_hits,
                                        int max_count,
                                        int* out_actual_count);

FinCore_API FinCore_Status FinCore_CheckRoundNumbers(void* session,
                                          const char* dataset,
                                          double threshold,      // e.g., 1000.0
                                          FinCore_DetectionHit* out_hits,
                                          int max_count,
                                          int* out_actual_count);

FinCore_API FinCore_Status FinCore_CheckOutliers(void* session,
                                      const char* dataset,
                                      double z_threshold,      // e.g., 3.0 for 3-sigma
                                      FinCore_DetectionHit* out_hits,
                                      int max_count,
                                      int* out_actual_count);
```

### 4.7 Import / Bulk Operations

```c
// Validates a batch of journal lines without posting
// Returns first error index in out_error_index, or FinCore_OK
FinCore_API FinCore_Status FinCore_ValidateBatch(void* session,
                                      const FinCore_JournalLine* lines,
                                      int line_count,
                                      int* out_error_index);

// Posts a validated batch atomically (all or nothing)
FinCore_API FinCore_Status FinCore_PostBatch(void* session,
                                  const FinCore_JournalLine* lines,
                                  int line_count);

// Returns summary statistics for a period
FinCore_API FinCore_Status FinCore_GetPeriodStats(void* session,
                                       const char* period_id,
                                       int* out_entry_count,
                                       int* out_line_count,
                                       double* out_total_debit,
                                       double* out_total_credit);
```


---

## 5. VB.NET Layer — Architecture

### 5.1 Project Structure

```
FinCoreAccountingGUI/
|-- FinCoreAccountingGUI.vbproj
|-- App.config
|
|-- Core/
|   |-- PInvoke/
|   |   |-- FinCoreNative.vb          ' All DllImport declarations
|   |   |-- Structs.vb            ' Blittable struct definitions
|   |   |-- Marshaling.vb         ' Helper: StringBuilder, IntPtr management
|   |
|   |-- Models/
|   |   |-- Account.vb            ' Domain wrapper for FinCore_Account
|   |   |-- JournalEntry.vb       ' Domain wrapper for FinCore_JournalLine
|   |   |-- Period.vb             ' Domain wrapper for FinCore_Period
|   |   |-- DetectionResult.vb    ' Domain wrapper for FinCore_DetectionHit
|   |
|   |-- Services/
|   |   |-- SessionService.vb     ' FinCore_CreateSession / DestroySession
|   |   |-- AccountService.vb     ' Chart CRUD operations
|   |   |-- JournalService.vb     ' Entry posting, retrieval
|   |   |-- ReportService.vb      ' GL, Balance, Bilan generation
|   |   |-- DetectionService.vb   ' Fraud detection orchestration
|   |   |-- PersistenceService.vb ' Save/Load session files
|   |
|   |-- Helpers/
|   |   |-- DateHelper.vb         ' ISO-8601 formatting
|   |   |-- CurrencyHelper.vb     ' Decimal <-> Double conversion
|   |   |-- ValidationHelper.vb   ' Input validation rules
|
|-- UI/
|   |-- Forms/
|   |   |-- MainForm.vb           ' MDI parent / dashboard
|   |   |-- LoginForm.vb          ' Session init (company, year, currency)
|   |   |-- AccountChartForm.vb   ' Plan comptable editor
|   |   |-- JournalEntryForm.vb   ' Manual entry (multi-line grid)
|   |   |-- GrandLivreForm.vb     ' GL viewer + export
|   |   |-- BalanceForm.vb        ' Trial balance viewer + export
|   |   |-- BilanForm.vb          ' Balance sheet viewer + export
|   |   |-- DetectionForm.vb      ' Fraud report viewer + export
|   |   |-- BenfordChartForm.vb   ' Distribution chart (Stretch)
|   |   |-- ImportForm.vb         ' Excel bulk import (Stretch)
|   |   |-- PeriodManagerForm.vb  ' Open/close periods
|   |
|   |-- Controls/
|   |   |-- AmountTextBox.vb      ' Currency input with validation
|   |   |-- AccountComboBox.vb    ' Auto-complete account selector
|   |   |-- DataGridJournal.vb    ' Custom DataGridView for journal lines
|   |   |-- ReportViewer.vb       ' Reusable report display panel
|   |
|   |-- Dialogs/
|   |   |-- ExportDialog.vb       ' Export options (format, range, filename)
|   |   |-- DetectionConfigDialog.vb ' Rule thresholds configuration
|   |   |-- AboutDialog.vb
|
|-- Excel/
|   |-- ClosedXmlExporter.vb      ' All .xlsx generation logic
|   |-- ClosedXmlImporter.vb      ' All .xlsx parsing logic (Stretch)
|   |-- Templates/
|   |   |-- GrandLivreTemplate.vb ' Column layout, headers, formatting
|   |   |-- BalanceTemplate.vb
|   |   |-- BilanTemplate.vb
|   |   |-- DetectionTemplate.vb
|   |-- Styles/
|   |   |-- CellStyles.vb         ' Fonts, borders, number formats
|   |   |-- ColorPalette.vb       ' Corporate colors
```

### 5.2 P/Invoke Declarations (FinCoreNative.vb)

```vb
' All strings marshaled as ANSI (LPStr) for C compatibility
' Structs use LayoutKind.Sequential, Pack:=8

Imports System.Runtime.InteropServices

Public Module FinCoreNative

    Private Const DLL_NAME As String = "FinCore_ae.dll"

    ' -- Lifecycle --
    <DllImport(DLL_NAME, CallingConvention:=CallingConvention.Cdecl, CharSet:=CharSet.Ansi)>
    Public Function FinCore_CreateSession(companyName As String,
                                       baseCurrency As String,
                                       fiscalYearStart As String) As IntPtr
    End Function

    <DllImport(DLL_NAME, CallingConvention:=CallingConvention.Cdecl)>
    Public Sub FinCore_DestroySession(session As IntPtr)
    End Sub

    <DllImport(DLL_NAME, CallingConvention:=CallingConvention.Cdecl, CharSet:=CharSet.Ansi)>
    Public Function FinCore_SaveSession(session As IntPtr, filepath As String) As Integer
    End Function

    <DllImport(DLL_NAME, CallingConvention:=CallingConvention.Cdecl, CharSet:=CharSet.Ansi)>
    Public Function FinCore_LoadSession(filepath As String) As IntPtr
    End Function

    <DllImport(DLL_NAME, CallingConvention:=CallingConvention.Cdecl, CharSet:=CharSet.Ansi)>
    Public Function FinCore_GetLastError(session As IntPtr) As IntPtr
    End Function

    ' -- Accounts --
    <DllImport(DLL_NAME, CallingConvention:=CallingConvention.Cdecl)>
    Public Function FinCore_SetAccount(session As IntPtr, ByRef account As FinCore_Account) As Integer
    End Function

    <DllImport(DLL_NAME, CallingConvention:=CallingConvention.Cdecl, CharSet:=CharSet.Ansi)>
    Public Function FinCore_GetAccount(session As IntPtr,
                                    code As String,
                                    ByRef outAccount As FinCore_Account) As Integer
    End Function

    <DllImport(DLL_NAME, CallingConvention:=CallingConvention.Cdecl, CharSet:=CharSet.Ansi)>
    Public Function FinCore_DeleteAccount(session As IntPtr, code As String) As Integer
    End Function

    <DllImport(DLL_NAME, CallingConvention:=CallingConvention.Cdecl)>
    Public Function FinCore_GetAccountCount(session As IntPtr) As Integer
    End Function

    <DllImport(DLL_NAME, CallingConvention:=CallingConvention.Cdecl)>
    Public Function FinCore_GetAllAccounts(session As IntPtr,
                                        <Out()> outAccounts As FinCore_Account(),
                                        maxCount As Integer,
                                        ByRef outActualCount As Integer) As Integer
    End Function

    <DllImport(DLL_NAME, CallingConvention:=CallingConvention.Cdecl)>
    Public Function FinCore_LoadStandardChart(session As IntPtr) As Integer
    End Function

    ' -- Periods --
    <DllImport(DLL_NAME, CallingConvention:=CallingConvention.Cdecl)>
    Public Function FinCore_CreatePeriod(session As IntPtr, ByRef period As FinCore_Period) As Integer
    End Function

    <DllImport(DLL_NAME, CallingConvention:=CallingConvention.Cdecl, CharSet:=CharSet.Ansi)>
    Public Function FinCore_GetPeriod(session As IntPtr,
                                   periodId As String,
                                   ByRef outPeriod As FinCore_Period) As Integer
    End Function

    <DllImport(DLL_NAME, CallingConvention:=CallingConvention.Cdecl, CharSet:=CharSet.Ansi)>
    Public Function FinCore_ClosePeriod(session As IntPtr, periodId As String) As Integer
    End Function

    <DllImport(DLL_NAME, CallingConvention:=CallingConvention.Cdecl, CharSet:=CharSet.Ansi)>
    Public Function FinCore_ReopenPeriod(session As IntPtr, periodId As String) As Integer
    End Function

    <DllImport(DLL_NAME, CallingConvention:=CallingConvention.Cdecl)>
    Public Function FinCore_GetPeriodCount(session As IntPtr) As Integer
    End Function

    ' -- Journal --
    <DllImport(DLL_NAME, CallingConvention:=CallingConvention.Cdecl)>
    Public Function FinCore_PostEntry(session As IntPtr,
                                   lines As FinCore_JournalLine(),
                                   lineCount As Integer) As Integer
    End Function

    <DllImport(DLL_NAME, CallingConvention:=CallingConvention.Cdecl, CharSet:=CharSet.Ansi)>
    Public Function FinCore_GetEntry(session As IntPtr,
                                  entryId As String,
                                  <Out()> outLines As FinCore_JournalLine(),
                                  maxCount As Integer,
                                  ByRef outActualCount As Integer) As Integer
    End Function

    <DllImport(DLL_NAME, CallingConvention:=CallingConvention.Cdecl, CharSet:=CharSet.Ansi)>
    Public Function FinCore_GetEntriesByDateRange(session As IntPtr,
                                               startDate As String,
                                               endDate As String,
                                               <Out()> outLines As FinCore_JournalLine(),
                                               maxCount As Integer,
                                               ByRef outActualCount As Integer) As Integer
    End Function

    <DllImport(DLL_NAME, CallingConvention:=CallingConvention.Cdecl, CharSet:=CharSet.Ansi)>
    Public Function FinCore_ReverseEntry(session As IntPtr,
                                      entryId As String,
                                      reversalDate As String,
                                      outReversalId As StringBuilder) As Integer
    End Function

    <DllImport(DLL_NAME, CallingConvention:=CallingConvention.Cdecl, CharSet:=CharSet.Ansi)>
    Public Function FinCore_DeleteEntry(session As IntPtr, entryId As String) As Integer
    End Function

    <DllImport(DLL_NAME, CallingConvention:=CallingConvention.Cdecl)>
    Public Function FinCore_GetJournalLineCount(session As IntPtr) As Integer
    End Function

    ' -- Reports --
    <DllImport(DLL_NAME, CallingConvention:=CallingConvention.Cdecl, CharSet:=CharSet.Ansi)>
    Public Function FinCore_GenerateGL(session As IntPtr,
                                    accountCode As String,
                                    startDate As String,
                                    endDate As String,
                                    <Out()> outLines As FinCore_GLLine(),
                                    maxCount As Integer,
                                    ByRef outActualCount As Integer) As Integer
    End Function

    <DllImport(DLL_NAME, CallingConvention:=CallingConvention.Cdecl, CharSet:=CharSet.Ansi)>
    Public Function FinCore_GenerateBalance(session As IntPtr,
                                         periodId As String,
                                         <Out()> outLines As FinCore_BalanceLine(),
                                         maxCount As Integer,
                                         ByRef outActualCount As Integer) As Integer
    End Function

    <DllImport(DLL_NAME, CallingConvention:=CallingConvention.Cdecl, CharSet:=CharSet.Ansi)>
    Public Function FinCore_GenerateBilan(session As IntPtr,
                                       asOfDate As String,
                                       <Out()> outLines As FinCore_BilanLine(),
                                       maxCount As Integer,
                                       ByRef outActualCount As Integer) As Integer
    End Function

    <DllImport(DLL_NAME, CallingConvention:=CallingConvention.Cdecl, CharSet:=CharSet.Ansi)>
    Public Function FinCore_GenerateCompteResultat(session As IntPtr,
                                                startDate As String,
                                                endDate As String,
                                                <Out()> outLines As FinCore_BilanLine(),
                                                maxCount As Integer,
                                                ByRef outActualCount As Integer) As Integer
    End Function

    ' -- Detection --
    <DllImport(DLL_NAME, CallingConvention:=CallingConvention.Cdecl, CharSet:=CharSet.Ansi)>
    Public Function FinCore_RunDetection(session As IntPtr,
                                      flags As Integer,
                                      dataset As String,
                                      ByRef outReport As FinCore_DetectionReport) As Integer
    End Function

    <DllImport(DLL_NAME, CallingConvention:=CallingConvention.Cdecl)>
    Public Sub FinCore_FreeDetectionReport(ByRef report As FinCore_DetectionReport)
    End Sub

    <DllImport(DLL_NAME, CallingConvention:=CallingConvention.Cdecl, CharSet:=CharSet.Ansi)>
    Public Function FinCore_GetBenfordDistribution(session As IntPtr,
                                                dataset As String,
                                                <Out()> outDigits As FinCore_BenfordDigit(),
                                                ByRef outSampleSize As Integer) As Integer
    End Function

    <DllImport(DLL_NAME, CallingConvention:=CallingConvention.Cdecl, CharSet:=CharSet.Ansi)>
    Public Function FinCore_CheckBenford(session As IntPtr,
                                      dataset As String,
                                      ByRef outChiSquare As Double,
                                      ByRef outPValue As Double,
                                      ByRef outSampleSize As Integer) As Integer
    End Function

    <DllImport(DLL_NAME, CallingConvention:=CallingConvention.Cdecl, CharSet:=CharSet.Ansi)>
    Public Function FinCore_CheckDuplicates(session As IntPtr,
                                         dataset As String,
                                         <Out()> outHits As FinCore_DetectionHit(),
                                         maxCount As Integer,
                                         ByRef outActualCount As Integer) As Integer
    End Function

    <DllImport(DLL_NAME, CallingConvention:=CallingConvention.Cdecl, CharSet:=CharSet.Ansi)>
    Public Function FinCore_CheckRoundNumbers(session As IntPtr,
                                           dataset As String,
                                           threshold As Double,
                                           <Out()> outHits As FinCore_DetectionHit(),
                                           maxCount As Integer,
                                           ByRef outActualCount As Integer) As Integer
    End Function

    <DllImport(DLL_NAME, CallingConvention:=CallingConvention.Cdecl, CharSet:=CharSet.Ansi)>
    Public Function FinCore_CheckOutliers(session As IntPtr,
                                       dataset As String,
                                       zThreshold As Double,
                                       <Out()> outHits As FinCore_DetectionHit(),
                                       maxCount As Integer,
                                       ByRef outActualCount As Integer) As Integer
    End Function

    ' -- Batch / Import --
    <DllImport(DLL_NAME, CallingConvention:=CallingConvention.Cdecl)>
    Public Function FinCore_ValidateBatch(session As IntPtr,
                                       lines As FinCore_JournalLine(),
                                       lineCount As Integer,
                                       ByRef outErrorIndex As Integer) As Integer
    End Function

    <DllImport(DLL_NAME, CallingConvention:=CallingConvention.Cdecl)>
    Public Function FinCore_PostBatch(session As IntPtr,
                                   lines As FinCore_JournalLine(),
                                   lineCount As Integer) As Integer
    End Function

    <DllImport(DLL_NAME, CallingConvention:=CallingConvention.Cdecl, CharSet:=CharSet.Ansi)>
    Public Function FinCore_GetPeriodStats(session As IntPtr,
                                        periodId As String,
                                        ByRef outEntryCount As Integer,
                                        ByRef outLineCount As Integer,
                                        ByRef outTotalDebit As Double,
                                        ByRef outTotalCredit As Double) As Integer
    End Function

End Module
```


### 5.3 Struct Definitions (Structs.vb)

```vb
Imports System.Runtime.InteropServices

<StructLayout(LayoutKind.Sequential, Pack:=8, CharSet:=CharSet.Ansi)>
Public Structure FinCore_Account
    <MarshalAs(UnmanagedType.ByValTStr, SizeConst:=16)>
    Public Code As String
    <MarshalAs(UnmanagedType.ByValTStr, SizeConst:=256)>
    Public Label As String
    Public Type As Integer
    Public Category As Integer
    <MarshalAs(UnmanagedType.ByValTStr, SizeConst:=16)>
    Public Parent As String
    Public OpeningBalance As Double
    Public IsActive As Integer
End Structure

<StructLayout(LayoutKind.Sequential, Pack:=8, CharSet:=CharSet.Ansi)>
Public Structure FinCore_JournalLine
    <MarshalAs(UnmanagedType.ByValTStr, SizeConst:=32)>
    Public EntryId As String
    <MarshalAs(UnmanagedType.ByValTStr, SizeConst:=11)>
    Public DateStr As String
    <MarshalAs(UnmanagedType.ByValTStr, SizeConst:=16)>
    Public AccountCode As String
    <MarshalAs(UnmanagedType.ByValTStr, SizeConst:=256)>
    Public Label As String
    <MarshalAs(UnmanagedType.ByValTStr, SizeConst:=64)>
    Public Reference As String
    Public Debit As Double
    Public Credit As Double
    <MarshalAs(UnmanagedType.ByValTStr, SizeConst:=4)>
    Public Currency As String
    Public ExchangeRate As Double
    <MarshalAs(UnmanagedType.ByValTStr, SizeConst:=16)>
    Public PeriodId As String
    <MarshalAs(UnmanagedType.ByValTStr, SizeConst:=32)>
    Public UserId As String
    <MarshalAs(UnmanagedType.ByValTStr, SizeConst:=20)>
    Public CreatedAt As String
End Structure

<StructLayout(LayoutKind.Sequential, Pack:=8, CharSet:=CharSet.Ansi)>
Public Structure FinCore_Period
    <MarshalAs(UnmanagedType.ByValTStr, SizeConst:=16)>
    Public PeriodId As String
    <MarshalAs(UnmanagedType.ByValTStr, SizeConst:=256)>
    Public Label As String
    <MarshalAs(UnmanagedType.ByValTStr, SizeConst:=11)>
    Public StartDate As String
    <MarshalAs(UnmanagedType.ByValTStr, SizeConst:=11)>
    Public EndDate As String
    Public Status As Integer
    <MarshalAs(UnmanagedType.ByValTStr, SizeConst:=32)>
    Public ClosedBy As String
    <MarshalAs(UnmanagedType.ByValTStr, SizeConst:=20)>
    Public ClosedAt As String
End Structure

<StructLayout(LayoutKind.Sequential, Pack:=8, CharSet:=CharSet.Ansi)>
Public Structure FinCore_GLLine
    <MarshalAs(UnmanagedType.ByValTStr, SizeConst:=11)>
    Public DateStr As String
    <MarshalAs(UnmanagedType.ByValTStr, SizeConst:=32)>
    Public EntryId As String
    <MarshalAs(UnmanagedType.ByValTStr, SizeConst:=64)>
    Public Reference As String
    <MarshalAs(UnmanagedType.ByValTStr, SizeConst:=256)>
    Public Label As String
    Public Debit As Double
    Public Credit As Double
    Public RunningBalance As Double
End Structure

<StructLayout(LayoutKind.Sequential, Pack:=8, CharSet:=CharSet.Ansi)>
Public Structure FinCore_BalanceLine
    <MarshalAs(UnmanagedType.ByValTStr, SizeConst:=16)>
    Public AccountCode As String
    <MarshalAs(UnmanagedType.ByValTStr, SizeConst:=256)>
    Public Label As String
    Public OpeningBalance As Double
    Public TotalDebit As Double
    Public TotalCredit As Double
    Public ClosingBalance As Double
    Public AccountType As Integer
End Structure

<StructLayout(LayoutKind.Sequential, Pack:=8, CharSet:=CharSet.Ansi)>
Public Structure FinCore_BilanLine
    <MarshalAs(UnmanagedType.ByValTStr, SizeConst:=64)>
    Public Section As String
    <MarshalAs(UnmanagedType.ByValTStr, SizeConst:=16)>
    Public AccountCode As String
    <MarshalAs(UnmanagedType.ByValTStr, SizeConst:=256)>
    Public Label As String
    Public GrossAmount As Double
    Public Depreciation As Double
    Public NetAmount As Double
    Public DisplayOrder As Integer
    Public IsTotalRow As Integer
    Public IndentLevel As Integer
End Structure

<StructLayout(LayoutKind.Sequential, Pack:=8, CharSet:=CharSet.Ansi)>
Public Structure FinCore_DetectionHit
    <MarshalAs(UnmanagedType.ByValTStr, SizeConst:=32)>
    Public EntryId As String
    <MarshalAs(UnmanagedType.ByValTStr, SizeConst:=16)>
    Public AccountCode As String
    <MarshalAs(UnmanagedType.ByValTStr, SizeConst:=11)>
    Public DateStr As String
    Public Amount As Double
    Public Flag As Integer
    Public Score As Double
    <MarshalAs(UnmanagedType.ByValTStr, SizeConst:=256)>
    Public Description As String
    <MarshalAs(UnmanagedType.ByValTStr, SizeConst:=256)>
    Public Recommendation As String
End Structure

<StructLayout(LayoutKind.Sequential, Pack:=8)>
Public Structure FinCore_DetectionReport
    Public HitCount As Integer
    Public Hits As IntPtr          ' Pointer to array of FinCore_DetectionHit
    Public BenfordChiSquare As Double
    Public BenfordPValue As Double
    Public BenfordSampleSize As Integer
End Structure

<StructLayout(LayoutKind.Sequential, Pack:=8)>
Public Structure FinCore_BenfordDigit
    Public Digit As Integer
    Public ExpectedFrequency As Double
    Public ActualFrequency As Double
    Public ActualCount As Integer
    Public Deviation As Double
End Structure
```

---

## 6. ClosedXML Export Specifications

### 6.1 Grand Livre Export

**File naming:** `GrandLivre_{accountCode}_{startDate}_{endDate}.xlsx`

**Worksheet:** "Grand Livre"

| Column | Header | Format | Width | Notes |
|--------|--------|--------|-------|-------|
| A | Date | Date (dd/MM/yyyy) | 12 | Left-aligned |
| B | N Ecriture | Text | 18 | Monospace font |
| C | Reference | Text | 20 | |
| D | Libelle | Text | 45 | Wrap text |
| E | Debit | Number (#,##0.00) | 15 | Right-aligned, red if > 0 |
| F | Credit | Number (#,##0.00) | 15 | Right-aligned, green if > 0 |
| G | Solde | Number (#,##0.00) | 15 | Bold running balance |

**Header Row:** Row 1, bold, background `#4472C4`, font white, freeze panes at row 2
**Sub-header:** Row 2 shows "Compte: {code} -- {label} -- Periode: {start} au {end}", merged A-G, italic
**Footer:** Last row shows "TOTAL", bold, with SUM formulas in E and F
**Page Setup:** Landscape, fit to width, repeat header row on print

### 6.2 Balance (Trial Balance) Export

**File naming:** `Balance_{periodId}.xlsx`

**Worksheet:** "Balance"

| Column | Header | Format | Width |
|--------|--------|--------|-------|
| A | Compte | Text | 12 |
| B | Libelle | Text | 40 |
| C | Solde d'Ouverture | Number (#,##0.00) | 18 |
| D | Total Debit | Number (#,##0.00) | 18 |
| E | Total Credit | Number (#,##0.00) | 18 |
| F | Solde de Cloture | Number (#,##0.00) | 18 |
| G | Type | Text | 12 |

**Conditional Formatting:**
- Closing balance > 0 and Type=Actif -> no color (normal)
- Closing balance < 0 and Type=Actif -> light red background (anomaly)
- Closing balance < 0 and Type=Passif -> no color (normal)
- Closing balance > 0 and Type=Passif -> light red background (anomaly)

**Totals Row:** SUM of D and E must equal. If not, highlight in red.

### 6.3 Bilan Export

**File naming:** `Bilan_{asOfDate}.xlsx`

**Two worksheets:** "ACTIF" and "PASSIF"

**ACTIF Worksheet:**

| Column | Header | Format | Width |
|--------|--------|--------|-------|
| A | Poste | Text | 45 | Indent based on IndentLevel |
| B | Brut | Number (#,##0.00) | 18 |
| C | Amort./Depr. | Number (#,##0.00) | 18 |
| D | Net | Number (#,##0.00) | 18 | Bold for total rows |

**Sections (display_order):**
1. ACTIF IMMOBILISE
   - Immobilisations incorporelles
   - Immobilisations corporelles
   - Immobilisations financieres
   - **Total Actif Immobilise**
2. ACTIF CIRCULANT
   - Stocks
   - Creances
   - Tresorerie actif
   - **Total Actif Circulant**
3. **TOTAL ACTIF**

**PASSIF Worksheet:** Mirror structure with Capitaux Propres, Dettes, Provisions.

**Cross-check:** Cell showing "TOTAL ACTIF" must equal cell showing "TOTAL PASSIF". If mismatch, red background + comment.

### 6.4 Detection Report Export

**File naming:** `Rapport_Detection_{date}_{flags}.xlsx`

**Worksheet 1:** "Resume" (Summary)

| Field | Value |
|-------|-------|
| Periode analysee | {dataset} |
| Date de generation | {now} |
| Nombre d'anomalies | {hit_count} |
| Score Benford x2 | {chi_square} |
| P-value Benford | {p_value} |
| Seuil significativite | 0.05 |
| Conclusion Benford | "Distribution conforme" or "ANOMALIE DETECTEE" |

**Worksheet 2:** "Detail des Anomalies"

| Column | Header | Format | Width |
|--------|--------|--------|-------|
| A | N Ecriture | Text | 18 |
| B | Date | Date | 12 |
| C | Compte | Text | 12 |
| D | Montant | Number | 15 |
| E | Regle declenchee | Text | 25 | "Benford", "Doublon", "Montant rond", "Outlier" |
| F | Score | Number (0.00%) | 12 | Severity percentage |
| G | Description | Text | 50 | Wrap text |
| H | Recommandation | Text | 50 | Wrap text |

**Conditional Formatting:**
- Score >= 0.80 -> Dark red background, white font
- Score 0.50-0.79 -> Orange background
- Score < 0.50 -> Yellow background

**Worksheet 3 (Stretch):** "Distribution Benford"

| Column | Header |
|--------|--------|
| A | Chiffre significatif (1-9) |
| B | Frequence attendue (%) |
| C | Frequence observee (%) |
| D | Ecart |
| E | Graphique (sparkline bar) |

### 6.5 Excel Import Specification (Stretch)

**Expected Input Format:** `.xlsx` with worksheet named "Ecritures"

| Column | Required | Format | Validation |
|--------|----------|--------|------------|
| A -- Date | Yes | Date or dd/MM/yyyy | Must be in open period |
| B -- Compte | Yes | Text, matches chart | Must exist and be active |
| C -- Libelle | Yes | Text, max 255 chars | Non-empty |
| D -- Reference | No | Text, max 63 chars | |
| E -- Debit | Yes* | Number >= 0 | Either E or F must be > 0 |
| F -- Credit | Yes* | Number >= 0 | Either E or F must be > 0 |
| G -- Devise | No | Text, 3 chars | Default to EUR |
| H -- Taux | No | Number > 0 | Default to 1.0 |

**Import Rules:**
- Rows are grouped by contiguous identical Date + Reference into a single entry
- Each group must balance (sum debit == sum credit)
- If a group doesn't balance, flag row and skip group
- Generate entry_id as `IMP-{timestamp}-{sequence}`
- Log all skipped rows with reason to import log


---

## 7. UI/UX Specifications

### 7.1 MainForm Layout

```
+-----------------------------------------------------------------+
|  FinCore Accounting Engine  |  Societe: [Name]  |  Exercice: [Year] |
+-----------------------------------------------------------------+
|  [Journal] [Grand Livre] [Balance] [Bilan] [Detection] [Import] |
+-----------------------------------------------------------------+
|                                                                 |
|                    [Active Module Panel]                        |
|                                                                 |
+-----------------------------------------------------------------+
|  Status: [Ready]  |  Periode active: [2024-01]  |  [Export v]   |
+-----------------------------------------------------------------+
```

**Color Scheme:**
- Primary: `#1F4E79` (deep blue)
- Secondary: `#4472C4` (medium blue)
- Accent: `#70AD47` (green for positive/credit)
- Warning: `#FFC000` (amber for alerts)
- Danger: `#C00000` (red for negative/debit/anomalies)
- Background: `#FFFFFF` / `#F2F2F2` alternating rows

### 7.2 JournalEntryForm

**Layout:**
```
+--------------------------------------------------------------+
|  Date: [__/__/____]  Reference: [________]  [Valider] [Annuler]|
+--------------------------------------------------------------+
|  Ligne | Compte          | Libelle      | Debit    | Credit   |
|  ------+---------+-------+--------------+----------+----------|
|   1    | [512    v]      | [__________] | [0.00]   | [0.00]   |
|   2    | [701    v]      | [__________] | [0.00]   | [0.00]   |
|   3    | [+ Ajouter ligne]                                          |
+--------------------------------------------------------------+
|  TOTAL:                              | [0.00]   | [0.00]   | [!]  |
+--------------------------------------------------------------+
```

**Validation:**
- Debit/Credit columns use `AmountTextBox` (numeric only, 2 decimals)
- AccountComboBox filters chart accounts by type as user types
- Total row shows running sums; turns green when balanced, red when unbalanced
- "Valider" disabled until total debit == total credit
- Minimum 2 lines required

### 7.3 DetectionForm

**Layout:**
```
+--------------------------------------------------------------+
|  Regles: [x Benford] [x Doublons] [x Montants ronds] [x Outliers]|
|  Periode: [Tout v]  [Lancer l'analyse]                       |
+--------------------------------------------------------------+
|  +-----------------+  +-------------------------------------+|
|  |  Score global   |  |  Distribution Benford (bar chart)   ||
|  |  [####..] 72%   |  |  [chart placeholder]                ||
|  |  12 anomalies   |  |                                     ||
|  +-----------------+  +-------------------------------------+|
+--------------------------------------------------------------+
|  [DataGridView: Detection hits with color-coded severity]    |
+--------------------------------------------------------------+
|  [Exporter en Excel]  [Generer rapport PDF]  [Fermer]        |
+--------------------------------------------------------------+
```

---

## 8. Benford's Law Implementation Specification

### 8.1 Algorithm

**Input:** Array of transaction amounts (absolute values, > 0)
**Output:** x2 statistic, p-value, per-digit frequencies

**Steps:**
1. Filter: amounts > 0 only
2. Extract leading digit: `d = floor(amount / 10^(floor(log10(amount))))`
3. Count occurrences per digit 1-9
4. Calculate expected: `E_d = N * log10(1 + 1/d)`
5. Calculate x2: `Sum((O_d - E_d)^2 / E_d)` for d = 1..9
6. Degrees of freedom: 8 (9 digits - 1)
7. p-value: `1 - CDF(x2, df=8)` using incomplete gamma function

**Minimum sample size:** 300 transactions (below this, warn "Echantillon insuffisant")

### 8.2 Interpretation Rules

| p-value | Interpretation | UI Color | Action |
|---------|----------------|----------|--------|
| > 0.10 | Distribution conforme | Green | None |
| 0.05 - 0.10 | Legere anomalie | Yellow | Review suggested |
| < 0.05 | Anomalie significative | Red | Detailed investigation required |

### 8.3 Per-Digit Thresholds (for granular alerts)

Flag a digit if: `|actual_frequency - expected_frequency| > 0.05` (5 percentage points)

---

## 9. Detection Rules Specification

### 9.1 Benford's Law (FinCore_DET_BENFORD)
- **Scope:** All transaction amounts in selected dataset
- **Trigger:** p-value < 0.05
- **Severity:** 1.0 (highest)
- **Description:** "La distribution des chiffres significatifs s'ecarte significativement de la loi de Benford. Possible manipulation des donnees."
- **Recommendation:** "Verifier les ecritures manuelles et les ajustements de fin d'exercice."

### 9.2 Duplicate Detection (FinCore_DET_DUPLICATE)
- **Scope:** Same amount, same account, within 7 days
- **Trigger:** Exact match on (amount, account_code) with different entry_id
- **Severity:** 0.7
- **Description:** "Ecriture en double detectee: meme montant et meme compte sur une periode de 7 jours."
- **Recommendation:** "Verifier s'il s'agit d'un paiement recurrent legitime ou d'un doublon de saisie."

### 9.3 Round Number Detection (FinCore_DET_ROUND_NUMBER)
- **Scope:** All transactions
- **Trigger:** Amount is a multiple of threshold (default: 1000.00) AND amount > threshold
- **Severity:** 0.4
- **Description:** "Montant rond detecte: {amount} est un multiple de {threshold}."
- **Recommendation:** "Les montants ronds peuvent indiquer des estimations ou des arrondis artificiels."

### 9.4 Outlier Detection (FinCore_DET_OUTLIER)
- **Scope:** Per account
- **Trigger:** Z-score > threshold (default: 3.0) within account
- **Calculation:** `z = (x - mu) / sigma` where mu and sigma are mean and stddev of account's historical amounts
- **Severity:** 0.6
- **Description:** "Montant aberrant pour le compte {code}: {amount} (ecart de {z} sigmas par rapport a la moyenne)."
- **Recommendation:** "Verifier la piece justificative et l'autorisation de cette ecriture exceptionnelle."

---

## 10. Persistence Specification

### 10.1 File Format

**Extension:** `.FinCore` (JSON-based, gzipped optional)
**Structure:**
```json
{
  "version": "1.0",
  "metadata": {
    "company_name": "Demo SARL",
    "base_currency": "EUR",
    "fiscal_year_start": "2024-01-01",
    "created_at": "2024-01-15T09:30:00Z",
    "last_modified": "2024-03-20T14:22:00Z"
  },
  "accounts": [
    {
      "code": "512",
      "label": "Banques",
      "type": 1,
      "category": 1,
      "parent": "51",
      "opening_balance": 15000.00,
      "is_active": 1
    }
  ],
  "periods": [
    {
      "period_id": "2024-01",
      "label": "Janvier 2024",
      "start_date": "2024-01-01",
      "end_date": "2024-01-31",
      "status": 1,
      "closed_by": "admin",
      "closed_at": "2024-02-05T10:00:00Z"
    }
  ],
  "journal": [
    {
      "entry_id": "E-2024-00001",
      "date": "2024-01-15",
      "account_code": "512",
      "label": "Vente client A",
      "reference": "FA-001",
      "debit": 0.00,
      "credit": 5000.00,
      "currency": "EUR",
      "exchange_rate": 1.0,
      "period_id": "2024-01",
      "user_id": "admin",
      "created_at": "2024-01-15T10:30:00Z"
    }
  ]
}
```

### 10.2 Auto-Save Behavior
- Auto-save every 5 minutes if dirty
- Auto-save on period close
- Manual save via Ctrl+S
- Backup: `.FinCore.bak` created before every overwrite


---

## 11. Feature Roadmap

### Phase 1 -- MVP (Weeks 1-3)
| # | Feature | C Engine | VB.NET GUI | Excel Export |
|---|---------|----------|------------|--------------|
| 1 | Session lifecycle (create/destroy/save/load) | Yes | Yes | -- |
| 2 | Standard chart loading | Yes | Yes | -- |
| 3 | Period management | Yes | Yes | -- |
| 4 | Manual journal entry (2+ lines, balanced) | Yes | Yes | -- |
| 5 | Grand Livre generation | Yes | Yes | Yes |
| 6 | Balance generation | Yes | Yes | Yes |
| 7 | Bilan generation | Yes | Yes | Yes |
| 8 | Benford's Law scoring | Yes | Yes | Yes |
| 9 | Detection report (Benford only) | Yes | Yes | Yes |
| 10 | Excel export (4 reports) | -- | -- | Yes |
| 11 | DLL/P-Invoke full wiring | Yes | Yes | -- |

### Phase 2 -- Stretch (Weeks 4-5, if MVP solid)
| # | Feature | Notes |
|---|---------|-------|
| 12 | Excel bulk import | ClosedXML parser, validation, batch posting |
| 13 | Duplicate detection rule | Per-account, 7-day window |
| 14 | Round-number detection rule | Configurable threshold |
| 15 | Outlier detection rule | Z-score per account |
| 16 | Benford distribution chart | Bar chart in DetectionForm |
| 17 | Multi-period comparison | Balance across 2+ periods, variance column |
| 18 | Compte de Resultat (P&L) | Charges vs Produits, margin calculation |

---

## 12. Demo Data Specification

### 12.1 Demo Company Profile

| Field | Value |
|-------|-------|
| Company | "SARL Demonstration" |
| SIRET | 123 456 789 00012 |
| Address | 1 Rue de la Comptabilite, 75001 Paris |
| Fiscal Year | 01/01/2024 - 31/12/2024 |
| Currency | EUR |
| Activity | Commerce de gros electronique |

### 12.2 Demo Chart of Accounts (Subset)

| Code | Label | Type | Opening Balance |
|------|-------|------|-----------------|
| 101 | Capital social | Passif | 50000.00 |
| 120 | Resultat de l'exercice | Passif | 0.00 |
| 211 | Terrains | Actif | 0.00 |
| 213 | Constructions | Actif | 120000.00 |
| 2813 | Amort. constructions | Passif | -36000.00 |
| 310 | Marchandises | Actif | 25000.00 |
| 401 | Fournisseurs | Passif | -15000.00 |
| 411 | Clients | Actif | 8000.00 |
| 512 | Banques | Actif | 35000.00 |
| 530 | Caisse | Actif | 500.00 |
| 607 | Achats marchandises | Charge | 0.00 |
| 621 | Sous-traitance | Charge | 0.00 |
| 641 | Remunerations du personnel | Charge | 0.00 |
| 701 | Ventes de marchandises | Produit | 0.00 |
| 706 | Services vendus | Produit | 0.00 |

### 12.3 Demo Journal Entries (20 entries, 40 lines)

**Entry 1 -- Capital injection:**
- 512 Banques +50 000 | 101 Capital social +50 000

**Entry 2 -- Purchase merchandise:**
- 310 Marchandises +12 500 | 401 Fournisseurs +12 500

**Entry 3 -- Sale to client:**
- 411 Clients +8 500 | 701 Ventes +8 500

**Entry 4 -- Salary payment:**
- 641 Remunerations +3 200 | 512 Banques +3 200

**Entry 5 -- Rent payment:**
- 613 Locations +1 200 | 512 Banques +1 200

**Entry 6 -- Bank loan:**
- 512 Banques +25 000 | 164 Emprunts +25 000

**Entry 7 -- Equipment purchase:**
- 215 Installations +15 000 | 512 Banques +15 000

**Entry 8 -- Client payment:**
- 512 Banques +5 000 | 411 Clients +5 000

**Entry 9 -- Supplier payment:**
- 401 Fournisseurs +7 500 | 512 Banques +7 500

**Entry 10 -- Service revenue:**
- 512 Banques +3 200 | 706 Services +3 200

**Entries 11-20:** Variations on the above with amounts designed to:
- Trigger Benford anomaly (manipulated leading digits)
- Include one exact duplicate (Entry 3 repeated)
- Include round numbers (10 000, 5 000, 1 000)
- Include one outlier (single transaction of 45 000 in account 512)

---

## 13. CV Narrative & Demo Script

### 13.1 One-Sentence Pitch
> "A Windows desktop accounting engine with embedded fraud detection, built in C as a reusable DLL and orchestrated through a VB.NET GUI -- demonstrating systems programming, cross-language interop, and applied statistical analysis in a real business domain."

### 13.2 Technical Competencies Demonstrated

| Competency | Evidence in Project |
|------------|---------------------|
| Systems Programming | C DLL with manual memory management, struct design, pointer arithmetic |
| Cross-Language Interop | Full P/Invoke layer: 25+ exported functions, blittable structs, string marshaling |
| Desktop GUI Development | VB.NET Windows Forms, custom controls, data binding, validation |
| File Format Engineering | ClosedXML for .xlsx generation: 4 report types with formatting, formulas, conditional styling |
| Applied Statistics | Benford's Law x2 test, Z-score outlier detection, statistical significance testing |
| Domain Knowledge | French PCG chart of accounts, double-entry bookkeeping, Grand Livre -> Balance -> Bilan pipeline |
| Software Architecture | Clean separation: C = pure logic, VB.NET = orchestration, no leakage across boundary |
| Testing & Validation | Input validation at both layers, balanced-entry enforcement, period-locking |

### 13.3 Demo Script (3 minutes)

**0:00-0:30 -- Setup & Context**
> "This is FinCore Accounting Engine, a desktop system I built for SME accounting with built-in fraud detection. The core is a C DLL handling all accounting math and detection logic. The GUI is VB.NET calling it through P/Invoke."

**0:30-1:00 -- Data Entry**
> "I'll create a journal entry: a sale to a client. The form enforces double-entry -- debit must equal credit. The account picker auto-completes from the French chart of accounts."

**1:00-1:45 -- Reports**
> "Now I'll generate the four standard reports. First, the Grand Livre -- the general ledger for account 512, our bank account. Then the trial balance, showing all accounts with their movements. Then the balance sheet, with assets and liabilities properly formatted. All export to real Excel files using ClosedXML -- no Excel installation needed."

**1:45-2:30 -- Fraud Detection**
> "Here's the detection engine. I'll run Benford's Law analysis on all transactions. The system extracts leading digits, compares against expected frequencies, and calculates a x2 statistic. With a p-value below 0.05, it flags potential data manipulation. The report exports to Excel with severity color-coding."

**2:30-3:00 -- Architecture Summary**
> "The C engine is pure logic -- no file I/O, no UI. The VB layer handles presentation and Excel export. This separation means the C DLL could be reused in a web service or mobile app tomorrow. The whole thing compiles to a single folder -- one EXE, one DLL, and it works on any Windows machine without installing Excel."

---

## 14. Build & Deployment

### 14.1 Build Outputs

```
/dist/
|-- FinCoreAccountingGUI.exe          ' VB.NET executable
|-- FinCore_ae.dll                    ' C core engine
|-- ClosedXML.dll                 ' Excel library
|-- DocumentFormat.OpenXml.dll    ' ClosedXML dependency
|-- ExcelNumberFormat.dll         ' ClosedXML dependency
|-- System.IO.Packaging.dll       ' OpenXML dependency
|-- demo_data.FinCore                 ' Pre-loaded demo session
```

### 14.2 Runtime Requirements
- Windows 10/11 (x64)
- .NET Framework 4.7.2+ OR .NET 6+ Windows Desktop runtime
- Visual C++ Redistributable (x64) -- for C DLL
- No Microsoft Office required

### 14.3 Development Environment
- **C compiler:** MSVC (Visual Studio 2022) or MinGW-w64
- **VB.NET:** Visual Studio 2022 with .NET desktop workload
- **NuGet packages:** ClosedXML (latest stable)
- **Testing:** xUnit for VB.NET service layer, custom C test runner for DLL

---

## 15. Error Handling Strategy

### 15.1 C Engine Error Model
- All functions return `FinCore_Status` (int)
- Error details stored per-session
- `FinCore_GetLastError()` returns human-readable string
- No exceptions -- pure C return codes

### 15.2 VB.NET Error Model
- Service layer wraps every P/Invoke call
- On `FinCore_Status != FinCore_OK`: throw custom `FinCoreException` with C error message
- UI layer catches `FinCoreException` and shows `MessageBox`
- All P/Invoke calls wrapped in `Try/Catch` with fallback to generic error

### 15.3 Common Error Scenarios

| Scenario | C Returns | VB.NET Action |
|----------|-----------|---------------|
| Unbalanced entry | `FinCore_ERR_UNBALANCED_ENTRY` | Highlight total row in red, show "Ecriture non equilibree" |
| Account not found | `FinCore_ERR_ACCOUNT_NOT_FOUND` | AccountComboBox shows red border, tooltip "Compte inexistant" |
| Period closed | `FinCore_ERR_PERIOD_CLOSED` | Disable "Valider", show lock icon |
| Invalid date | `FinCore_ERR_INVALID_INPUT` | DatePicker shows red border |
| File not found | `FinCore_ERR_FILE_IO` | Show file dialog to locate file |
| Out of memory | `FinCore_ERR_OUT_OF_MEMORY` | Show critical error, suggest restart |
| Duplicate key | `FinCore_ERR_DUPLICATE_KEY` | Highlight conflicting field, show existing value |

---

## 16. Testing Strategy

### 16.1 C Engine Unit Tests

| Test Suite | Coverage |
|------------|----------|
| Session lifecycle | Create, save, load, destroy -- verify no memory leaks |
| Account chart | Add, get, delete, load standard -- verify constraints |
| Journal posting | Balanced entries, unbalanced rejection, period validation |
| Grand Livre | Verify running balance accuracy against manual calculation |
| Balance | Verify total debit == total credit, closing balance math |
| Bilan | Verify ACTIF == PASSIF, section totals |
| Benford | Test with known distributions (conformant vs manipulated) |
| Detection rules | Each rule with synthetic data designed to trigger it |
| Boundary | Empty datasets, single transaction, max limits |

### 16.2 VB.NET Integration Tests

| Test Suite | Coverage |
|------------|----------|
| P/Invoke marshaling | Round-trip all struct types, verify field integrity |
| Service layer | Each service method with mock C responses |
| Excel export | Generate files, verify formulas, open in Excel validator |
| Excel import (Stretch) | Parse known-good files, reject malformed files |
| UI validation | Form field validation, button state changes |
| End-to-end | Full workflow: create session -> post entries -> generate reports -> export |

### 16.3 Performance Benchmarks

| Metric | Target | Test |
|--------|--------|------|
| Session load | < 500ms | 1000 accounts, 10000 journal lines |
| GL generation | < 200ms | Single account, full year |
| Balance generation | < 300ms | All accounts, full year |
| Benford analysis | < 500ms | 10000 transactions |
| Excel export | < 2s | 5000-row Grand Livre |
| Memory footprint | < 128MB | Full demo dataset loaded |

---

## 17. Security Considerations

| Concern | Mitigation |
|---------|------------|
| DLL hijacking | Sign DLL, verify hash on load |
| Session file tampering | Optional HMAC signature on .FinCore files |
| Input injection | All strings bounded, null-terminated in C |
| Numeric overflow | Use double (IEEE 754) with range checks |
| Path traversal | Validate and sanitize all file paths |
| Multi-user (future) | Session is single-user; no auth in MVP |

---

## 18. Future Extensions (Post-MVP)

| Feature | Description | Complexity |
|---------|-------------|------------|
| Multi-company | Switch between multiple session files | Low |
| Multi-currency | Real-time exchange rates, revaluation | Medium |
| VAT handling | Auto-calculate TVA on entries | Medium |
| Bank reconciliation | Import bank statements, match entries | High |
| Audit trail | Immutable log of all changes | Medium |
| REST API | Wrap C DLL in HTTP service | High |
| Web GUI | Blazor or React frontend | High |
| SQL persistence | Replace flat-file with SQLite/PostgreSQL | Medium |
| Machine learning | Anomaly detection beyond rule-based | High |

---

## 19. Glossary

| Term | Definition |
|------|------------|
| **Grand Livre** | General ledger -- chronological record of all transactions per account |
| **Balance** | Trial balance -- summary of all accounts with debit/credit totals |
| **Bilan** | Balance sheet -- snapshot of assets, liabilities, equity at a point in time |
| **Compte de Resultat** | Income statement -- revenues vs expenses over a period |
| **Plan Comptable** | Chart of accounts -- standardized list of account codes (French PCG) |
| **Ecriture** | Journal entry -- a balanced set of debit/credit lines |
| **Piece justificative** | Supporting document/reference for a transaction |
| **Benford's Law** | Statistical law about frequency distribution of leading digits |
| **P/Invoke** | Platform Invoke -- .NET mechanism to call unmanaged DLL functions |
| **Blittable** | Data types that have identical memory representation in managed and unmanaged code |
| **ClosedXML** | .NET library for reading/writing Excel files without Office Interop |

---

## 20. Document History

| Version | Date | Author | Changes |
|---------|------|--------|---------|
| 1.0 | 2024-08-22 | Project Lead | Initial specification -- MVP + Stretch scope, full API, UI specs, demo data |

---

*End of Project Explication Book*
