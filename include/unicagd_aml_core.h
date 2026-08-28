#ifndef UNICAGD_AML_CORE_H
#define UNICAGD_AML_CORE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#define UNICAGD_AML_VERSION_MAJOR 1
#define UNICAGD_AML_VERSION_MINOR 3
#define UNICAGD_AML_VERSION_PATCH 0
#define UNICAGD_MAX_STR_LEN 64
#define UNICAGD_MAX_REASON_LEN 256
#define UNICAGD_MAX_ALERTS 65536

typedef enum {
    SEVERITY_LOW = 0,
    SEVERITY_MEDIUM = 1,
    SEVERITY_HIGH = 2,
    SEVERITY_CRITICAL = 3
} unicagd_severity_t;

typedef enum {
    CATEGORY_SMURFING_STRUCTURING = 0,
    CATEGORY_FANIN_AGGREGATION = 1,
    CATEGORY_RAPID_PASSTHROUGH = 2,
    CATEGORY_CIRCULAR_LAYERING = 3,
    CATEGORY_HIGH_RISK_JURISDICTION = 4,
    CATEGORY_ACCOUNT_TAKEOVER = 5,
    CATEGORY_APP_SCAM = 6,
    CATEGORY_CARD_ANOMALY = 7
} unicagd_category_t;

typedef enum {
    THEME_MINIMAL = 0,
    THEME_CYBERPUNK = 1,
    THEME_MATRIX = 2,
    THEME_MONOKAI = 3,
    THEME_PACMAN = 4
} unicagd_theme_t;

typedef struct {
    char transaction_id[UNICAGD_MAX_STR_LEN];
    char originator_account[UNICAGD_MAX_STR_LEN];
    char beneficiary_account[UNICAGD_MAX_STR_LEN];
    double amount;
    char currency[8];
    uint64_t timestamp_ms;
    char ip_address[32];
    char device_id[32];
    double lat;
    double lon;
    char country[8];
    char channel[16];
    bool is_card_transaction;
} unicagd_transaction_t;

typedef struct {
    char alert_id[UNICAGD_MAX_STR_LEN];
    char rule_id[16];
    char rule_name[UNICAGD_MAX_STR_LEN];
    unicagd_category_t category;
    unicagd_severity_t severity;
    char subject_account[UNICAGD_MAX_STR_LEN];
    char counterparty_account[UNICAGD_MAX_STR_LEN];
    double total_suspicious_amount;
    char currency[8];
    double confidence_score;
    char reason[UNICAGD_MAX_REASON_LEN];
    char typology[UNICAGD_MAX_STR_LEN];
    uint64_t timestamp_ms;
} unicagd_alert_t;

typedef struct {
    uint64_t total_transactions_analyzed;
    double total_volume_analyzed;
    uint32_t alerts_count;
    uint32_t critical_alerts;
    uint32_t high_alerts;
    uint32_t medium_alerts;
    uint32_t low_alerts;
    double total_suspicious_amount;
    uint32_t suspicious_accounts_count;
    double execution_time_seconds;
    double throughput_tx_per_sec;
    char primary_hardware[64];
    unicagd_alert_t* alerts;
} unicagd_report_t;

// Core Engine Lifecycle
int unicagd_aml_init(void);
void unicagd_aml_shutdown(void);

// Evaluation API
unicagd_report_t* unicagd_aml_evaluate(
    const unicagd_transaction_t* transactions,
    size_t count,
    const char* hardware_target
);
void unicagd_aml_free_report(unicagd_report_t* report);

// Turbocharged Simulation Generator
size_t unicagd_aml_generate_synthetic_stream(
    unicagd_transaction_t* buffer,
    size_t capacity,
    double fraud_ratio,
    uint64_t seed
);

// High-Standard TUI Renderer
void unicagd_tui_set_theme(unicagd_theme_t theme);
void unicagd_tui_render_banner(const char* title, const char* hardware_info);
void unicagd_tui_render_progress(
    uint64_t current,
    uint64_t total,
    double elapsed_sec,
    double tx_per_sec,
    uint32_t alert_count,
    double flagged_volume
);
void unicagd_tui_render_log_entry(
    const unicagd_transaction_t* tx,
    const unicagd_alert_t* alert_opt
);
void unicagd_tui_render_final_report(const unicagd_report_t* report);

// Multilingual HTML Export
int unicagd_export_html_report(
    const unicagd_report_t* report,
    const char* output_filepath,
    const char* language_code // "hu" or "en"
);

#ifdef __cplusplus
}
#endif

#endif // UNICAGD_AML_CORE_H
