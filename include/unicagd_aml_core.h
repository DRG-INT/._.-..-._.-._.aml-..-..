#ifndef UNICAGD_AML_CORE_H
#define UNICAGD_AML_CORE_H

/*
 * UNICAGD_AML Core Header - Public API.
 * UNICAGD_AML Core Fejléc - Nyilvános API.
 *
 * This header defines the public interface for the UNICAGD_AML engine.
 * Ez a fejléc definiálja a UNICAGD_AML motor nyilvános interfészét.
 */
#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/*
 * Library version macros.
 * Könyvtár verzió makrók.
 */
#define UNICAGD_AML_VERSION_MAJOR 1    /* Major version / Fő verzió */
#define UNICAGD_AML_VERSION_MINOR 3    /* Minor version / Alverzió */
#define UNICAGD_AML_VERSION_PATCH 0    /* Patch version / Javítás verzió */
#define UNICAGD_MAX_STR_LEN 64         /* Maximum string length / Maximális string hossz */
#define UNICAGD_MAX_REASON_LEN 256     /* Maximum reason length / Maximális indoklás hossz */
#define UNICAGD_MAX_ALERTS 65536       /* Maximum alerts / Maximális riasztások */

/*
 * Alert severity levels.
 * Riasztási súlyossági szintek.
 */
typedef enum {
    SEVERITY_LOW = 0,      /* Low severity / Alacsony súlyosság */
    SEVERITY_MEDIUM = 1,   /* Medium severity / Közép súlyosság */
    SEVERITY_HIGH = 2,     /* High severity / Magas súlyosság */
    SEVERITY_CRITICAL = 3  /* Critical severity / Kritikus súlyosság */
} unicagd_severity_t;

/*
 * Alert category types.
 * Riasztási kategóriatípusok.
 */
typedef enum {
    CATEGORY_SMURFING_STRUCTURING = 0,    /* Structuring/smurfing / Szerkeztetés */
    CATEGORY_FANIN_AGGREGATION = 1,       /* Fan-in aggregation / Gyűjtés */
    CATEGORY_RAPID_PASSTHROUGH = 2,       /* Rapid pass-through / Gyors továbbítás */
    CATEGORY_CIRCULAR_LAYERING = 3,       /* Circular layering / Kör alakú rétegezés */
    CATEGORY_HIGH_RISK_JURISDICTION = 4,  /* High-risk jurisdiction / Kockázatos joghatóság */
    CATEGORY_ACCOUNT_TAKEOVER = 5,        /* Account takeover / Számlaelfoglalás */
    CATEGORY_APP_SCAM = 6,                /* App scam / Alkalmazás csalás */
    CATEGORY_CARD_ANOMALY = 7             /* Card anomaly / Kártya anomália */
} unicagd_category_t;

/*
 * TUI theme options.
 * TUI téma lehetőségek.
 */
typedef enum {
    THEME_MINIMAL = 0,    /* Minimal theme / Minimal téma */
    THEME_CYBERPUNK = 1,  /* Cyberpunk theme / Cyberpunk téma */
    THEME_MATRIX = 2,     /* Matrix theme / Mátrix téma */
    THEME_MONOKAI = 3,    /* Monokai theme / Monokai téma */
    THEME_PACMAN = 4      /* Pacman theme / Pacman téma */
} unicagd_theme_t;

/*
 * Transaction data structure.
 * Tranzakció adatstruktúra.
 */
typedef struct {
    char transaction_id[UNICAGD_MAX_STR_LEN];        /* Unique transaction ID / Egyedi tranzakció azonosító */
    char originator_account[UNICAGD_MAX_STR_LEN];    /* Sender account / Küldő számla */
    char beneficiary_account[UNICAGD_MAX_STR_LEN];   /* Receiver account / Címzett számla */
    double amount;                                   /* Transaction amount / Tranzakció összege */
    char currency[8];                                /* Currency code / Pénznem kód */
    uint64_t timestamp_ms;                           /* Timestamp in ms / Időbélyeg ms-ben */
    char ip_address[32];                             /* IP address / IP cím */
    char device_id[32];                              /* Device ID / Eszköz azonosító */
    double lat;                                      /* Latitude / Szélesség */
    double lon;                                      /* Longitude / Hosszúság */
    char country[8];                                 /* Country code / Ország kód */
    char channel[16];                                /* Transaction channel / Tranzakció csatorna */
    bool is_card_transaction;                        /* Is card transaction / Kártya tranzakció-e */
} unicagd_transaction_t;

/*
 * Alert data structure.
 * Riasztási adatstruktúra.
 */
typedef struct {
    char alert_id[UNICAGD_MAX_STR_LEN];          /* Unique alert ID / Egyedi riasztás azonosító */
    char rule_id[16];                             /* Rule identifier / Szabály azonosító */
    char rule_name[UNICAGD_MAX_STR_LEN];          /* Rule name / Szabály név */
    unicagd_category_t category;                  /* Alert category / Riasztás kategória */
    unicagd_severity_t severity;                  /* Alert severity / Riasztás súlyossága */
    char subject_account[UNICAGD_MAX_STR_LEN];   /* Subject account / Tárgy számla */
    char counterparty_account[UNICAGD_MAX_STR_LEN]; /* Counterparty account / Ellenfél számla */
    double total_suspicious_amount;               /* Suspicious amount / Gyanús összeg */
    char currency[8];                             /* Currency code / Pénznem kód */
    double confidence_score;                      /* Confidence score / Megbízhatósági pontszám */
    char reason[UNICAGD_MAX_REASON_LEN];          /* Alert reason / Riasztás indoklása */
    char typology[UNICAGD_MAX_STR_LEN];           /* Crime typology / Bűncselekmény tipológia */
    uint64_t timestamp_ms;                        /* Transaction timestamp / Tranzakció időbélyege */
} unicagd_alert_t;

/*
 * Report data structure.
 * Jelentés adatstruktúra.
 */
typedef struct {
    uint64_t total_transactions_analyzed;     /* Total transactions / Feldolgozott tranzakciók */
    double total_volume_analyzed;             /* Total volume / Teljes forgalom */
    uint32_t alerts_count;                    /* Total alerts / Riasztások száma */
    uint32_t critical_alerts;                 /* Critical alerts / Kritikus riasztások */
    uint32_t high_alerts;                     /* High alerts / Magas riasztások */
    uint32_t medium_alerts;                   /* Medium alerts / Közép riasztások */
    uint32_t low_alerts;                      /* Low alerts / Alacsony riasztások */
    double total_suspicious_amount;           /* Suspicious amount / Gyanús összeg */
    uint32_t suspicious_accounts_count;       /* Suspicious accounts / Gyanús számlák */
    double execution_time_seconds;            /* Processing time / Feldolgozási idő */
    double throughput_tx_per_sec;             /* Throughput / Áteresztőképesség */
    char primary_hardware[64];                /* Hardware used / Használt hardver */
    unicagd_alert_t* alerts;                  /* Alerts array / Riasztások tömbje */
} unicagd_report_t;

/*
 * Core engine lifecycle functions.
 * Fő motor életciklus függvények.
 */
int unicagd_aml_init(void);                                    /* Initialize engine / Motor inicializálása */
void unicagd_aml_shutdown(void);                               /* Shutdown engine / Motor leállítása */

/*
 * Evaluation API.
 * Értékelési API.
 */
unicagd_report_t* unicagd_aml_evaluate(                       /* Evaluate transactions / Tranzakciók értékelése */
    const unicagd_transaction_t* transactions,
    size_t count,
    const char* hardware_target
);
void unicagd_aml_free_report(unicagd_report_t* report);        /* Free report / Jelentés felszabadítása */

/*
 * Turbocharged simulation generator.
 * Turbófeltöltős szimuláció generátor.
 */
size_t unicagd_aml_generate_synthetic_stream(                 /* Generate synthetic stream / Szintetikus folyam generálása */
    unicagd_transaction_t* buffer,
    size_t capacity,
    double fraud_ratio,
    uint64_t seed
);

/*
 * High-standard TUI renderer.
 * Magas színvonalú TUI megjelenítő.
 */
void unicagd_tui_set_theme(unicagd_theme_t theme);                             /* Set theme / Téma beállítása */
void unicagd_tui_render_banner(const char* title, const char* hardware_info);  /* Render banner / Banner megjelenítése */
void unicagd_tui_render_progress(                                             /* Render progress / Folyamat megjelenítése */
    uint64_t current,
    uint64_t total,
    double elapsed_sec,
    double tx_per_sec,
    uint32_t alert_count,
    double flagged_volume
);
void unicagd_tui_render_log_entry(                                            /* Render log entry / Napló megjelenítése */
    const unicagd_transaction_t* tx,
    const unicagd_alert_t* alert_opt
);
void unicagd_tui_render_final_report(const unicagd_report_t* report);         /* Render report / Jelentés megjelenítése */

/*
 * Multilingual HTML export.
 * Többnyelvű HTML export.
 */
int unicagd_export_html_report(                                               /* Export HTML report / HTML jelentés exportálása */
    const unicagd_report_t* report,
    const char* output_filepath,
    const char* language_code // "hu" or "en"
);

#ifdef __cplusplus
}
#endif

#endif // UNICAGD_AML_CORE_H
