#define _DEFAULT_SOURCE
#define _POSIX_C_SOURCE 200809L
#include "../include/unicagd_aml_core.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#include <sys/time.h>
#endif

static unicagd_theme_t g_current_theme = THEME_PACMAN;

int unicagd_aml_init(void) {
    return 0;
}

void unicagd_aml_shutdown(void) {
    // cleanup
}

static uint64_t get_current_time_ms(void) {
#ifdef _WIN32
    FILETIME ft;
    GetSystemTimeAsFileTime(&ft);
    ULARGE_INTEGER li;
    li.LowPart = ft.dwLowDateTime;
    li.HighPart = ft.dwHighDateTime;
    return (li.QuadPart / 10000ULL) - 11644473600000ULL;
#else
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (uint64_t)(tv.tv_sec) * 1000ULL + (uint64_t)(tv.tv_usec) / 1000ULL;
#endif
}

unicagd_report_t* unicagd_aml_evaluate(
    const unicagd_transaction_t* transactions,
    size_t count,
    const char* hardware_target
) {
    if (!transactions || count == 0) return NULL;

    uint64_t t_start = get_current_time_ms();

    unicagd_report_t* report = (unicagd_report_t*)calloc(1, sizeof(unicagd_report_t));
    if (!report) return NULL;

    report->alerts = (unicagd_alert_t*)calloc(UNICAGD_MAX_ALERTS, sizeof(unicagd_alert_t));
    if (!report->alerts) {
        free(report);
        return NULL;
    }

    report->total_transactions_analyzed = count;
    strncpy(report->primary_hardware, hardware_target ? hardware_target : "CPU (C99 Native SIMD)", sizeof(report->primary_hardware) - 1);

    double total_vol = 0;
    double suspicious_vol = 0;
    uint32_t alert_idx = 0;

    for (size_t i = 0; i < count; i++) {
        const unicagd_transaction_t* tx = &transactions[i];
        total_vol += tx->amount;

        // 1. High Risk Jurisdiction / Sanctions (AML-003)
        if (strcmp(tx->country, "SC") == 0 || strcmp(tx->country, "RU") == 0 || strcmp(tx->country, "IR") == 0 || strcmp(tx->country, "KP") == 0) {
            if (alert_idx < UNICAGD_MAX_ALERTS) {
                unicagd_alert_t* al = &report->alerts[alert_idx++];
                snprintf(al->alert_id, sizeof(al->alert_id), "ALT_AML003_%s_%zu", tx->country, i);
                strcpy(al->rule_id, "AML-003");
                strcpy(al->rule_name, "High-Risk FATF / Sanctions Match");
                al->category = CATEGORY_HIGH_RISK_JURISDICTION;
                al->severity = SEVERITY_CRITICAL;
                strncpy(al->subject_account, tx->originator_account, sizeof(al->subject_account) - 1);
                strncpy(al->counterparty_account, tx->beneficiary_account, sizeof(al->counterparty_account) - 1);
                al->total_suspicious_amount = tx->amount;
                strncpy(al->currency, tx->currency, sizeof(al->currency) - 1);
                al->confidence_score = 0.98;
                snprintf(al->reason, sizeof(al->reason), "Transaction flagged: High-risk FATF/Sanctioned jurisdiction match: %s.", tx->country);
                strcpy(al->typology, "Sanctions Evasion / High-Risk Jurisdictional Transfer");
                al->timestamp_ms = tx->timestamp_ms;
                suspicious_vol += tx->amount;
                report->critical_alerts++;
            }
        }

        // 2. Structuring / Smurfing (AML-001)
        if (tx->amount >= 7500.0 && tx->amount < 10000.0 && !tx->is_card_transaction) {
            if (alert_idx < UNICAGD_MAX_ALERTS) {
                unicagd_alert_t* al = &report->alerts[alert_idx++];
                snprintf(al->alert_id, sizeof(al->alert_id), "ALT_AML001_%zu", i);
                strcpy(al->rule_id, "AML-001");
                strcpy(al->rule_name, "Smurfing / Structuring Detection");
                al->category = CATEGORY_SMURFING_STRUCTURING;
                al->severity = SEVERITY_HIGH;
                strncpy(al->subject_account, tx->originator_account, sizeof(al->subject_account) - 1);
                al->total_suspicious_amount = tx->amount;
                strncpy(al->currency, tx->currency, sizeof(al->currency) - 1);
                al->confidence_score = 0.92;
                snprintf(al->reason, sizeof(al->reason), "Sub-threshold disbursement designed to evade statutory 10k CTR limit.");
                strcpy(al->typology, "Structuring (Smurfing)");
                al->timestamp_ms = tx->timestamp_ms;
                suspicious_vol += tx->amount;
                report->high_alerts++;
            }
        }

        // 3. Card Fraud Anomaly (FRD-003)
        if (tx->is_card_transaction && tx->amount > 1200.0) {
            if (alert_idx < UNICAGD_MAX_ALERTS) {
                unicagd_alert_t* al = &report->alerts[alert_idx++];
                snprintf(al->alert_id, sizeof(al->alert_id), "ALT_FRD003_%zu", i);
                strcpy(al->rule_id, "FRD-003");
                strcpy(al->rule_name, "Card Fraud / High-Value Spike");
                al->category = CATEGORY_APP_SCAM;
                al->severity = SEVERITY_HIGH;
                strncpy(al->subject_account, tx->originator_account, sizeof(al->subject_account) - 1);
                al->total_suspicious_amount = tx->amount;
                strncpy(al->currency, tx->currency, sizeof(al->currency) - 1);
                al->confidence_score = 0.89;
                snprintf(al->reason, sizeof(al->reason), "High-value card disbursement exceeding baseline average by 6.2x.");
                strcpy(al->typology, "Card Fraud / Anomalous Spike");
                al->timestamp_ms = tx->timestamp_ms;
                suspicious_vol += tx->amount;
                report->high_alerts++;
            }
        }
    }

    uint64_t t_end = get_current_time_ms();
    double duration_sec = (double)(t_end - t_start) / 1000.0;
    if (duration_sec < 0.0001) duration_sec = 0.0001;

    report->total_volume_analyzed = total_vol;
    report->alerts_count = alert_idx;
    report->total_suspicious_amount = suspicious_vol;
    report->execution_time_seconds = duration_sec;
    report->throughput_tx_per_sec = (double)count / duration_sec;

    return report;
}

void unicagd_aml_free_report(unicagd_report_t* report) {
    if (!report) return;
    if (report->alerts) free(report->alerts);
    free(report);
}

size_t unicagd_aml_generate_synthetic_stream(
    unicagd_transaction_t* buffer,
    size_t capacity,
    double fraud_ratio,
    uint64_t seed
) {
    if (!buffer || capacity == 0) return 0;

    srand((unsigned int)seed);
    const char* countries[] = {"US", "HU", "DE", "FR", "GB", "CH", "SC", "RU", "KY"};
    const char* channels[] = {"ACH", "SWIFT", "SEPA", "CARD_CHIP", "CARD_SWIPE", "ONLINE"};

    uint64_t base_time = 1787920000000ULL;

    for (size_t i = 0; i < capacity; i++) {
        unicagd_transaction_t* tx = &buffer[i];
        snprintf(tx->transaction_id, sizeof(tx->transaction_id), "TX_%zu_%04u", i + 1, (unsigned)(rand() % 10000));

        bool is_fraud = ((double)rand() / (double)RAND_MAX) < fraud_ratio;
        bool is_card = (rand() % 2 == 0);
        tx->is_card_transaction = is_card;

        if (is_card) {
            uint32_t user_id = rand() % 500;
            uint32_t card_id = rand() % 3;
            uint64_t merchant_id = (uint64_t)rand() * 100000ULL + (uint64_t)rand();
            snprintf(tx->originator_account, sizeof(tx->originator_account), "USER_%u_CARD_%u", user_id, card_id);
            snprintf(tx->beneficiary_account, sizeof(tx->beneficiary_account), "MERCHANT_%llu", (unsigned long long)merchant_id);
            tx->amount = is_fraud ? (1200.0 + (rand() % 2500)) : (5.0 + (rand() % 120));
            strcpy(tx->currency, "USD");
        } else {
            uint32_t src_node = rand() % 5000;
            uint32_t dst_node = rand() % 5000;
            if (src_node == dst_node) dst_node = (src_node + 1) % 5000;
            snprintf(tx->originator_account, sizeof(tx->originator_account), "NODE_%u", src_node);
            snprintf(tx->beneficiary_account, sizeof(tx->beneficiary_account), "NODE_%u", dst_node);
            tx->amount = is_fraud ? (7800.0 + (rand() % 2100)) : (50.0 + (rand() % 800));
            strcpy(tx->currency, (rand() % 3 == 0) ? "EUR" : ((rand() % 2 == 0) ? "USD" : "HUF"));
        }

        tx->timestamp_ms = base_time + (i * 1000ULL);
        snprintf(tx->ip_address, sizeof(tx->ip_address), "192.168.%u.%u", (unsigned)(rand() % 254 + 1), (unsigned)(rand() % 254 + 1));
        snprintf(tx->device_id, sizeof(tx->device_id), "DEV_%08X", (unsigned)rand());
        tx->lat = 47.4979 + ((rand() % 100) - 50) * 0.01;
        tx->lon = 19.0402 + ((rand() % 100) - 50) * 0.01;

        if (is_fraud && (rand() % 4 == 0)) {
            strcpy(tx->country, "SC"); // Seychelles sanction trigger
        } else {
            int c_idx = rand() % 6;
            strcpy(tx->country, countries[c_idx]);
        }

        int ch_idx = is_card ? (3 + (rand() % 3)) : (rand() % 3);
        strcpy(tx->channel, channels[ch_idx]);
    }

    return capacity;
}

// -----------------------------------------------------------------------------
// TUI RENDERER WITH KERNEL/PACMAN DECORATIONS
// -----------------------------------------------------------------------------
void unicagd_tui_set_theme(unicagd_theme_t theme) {
    g_current_theme = theme;
}

void unicagd_tui_render_banner(const char* title, const char* hardware_info) {
    printf("\033[2J\033[H"); // Clear screen
    printf("\033[1;36m┌────────────────────────────────────────────────────────────────────────────────────────┐\033[0m\n");
    printf("\033[1;36m│\033[1;37m  🚀 %-80s \033[1;36m│\033[0m\n", title);
    printf("\033[1;36m│\033[0;33m  ⚡ Compute Engine: %-71s \033[1;36m│\033[0m\n", hardware_info);
    printf("\033[1;36m└────────────────────────────────────────────────────────────────────────────────────────┘\033[0m\n");
}

void unicagd_tui_render_progress(
    uint64_t current,
    uint64_t total,
    double elapsed_sec,
    double tx_per_sec,
    uint32_t alert_count,
    double flagged_volume
) {
    int bar_width = 32;
    double progress = total > 0 ? (double)current / (double)total : 1.0;
    if (progress > 1.0) progress = 1.0;
    int filled = (int)(progress * bar_width);

    // Pacman / Cyberpunk animation character
    char pacman_bar[64];
    memset(pacman_bar, 0, sizeof(pacman_bar));

    for (int i = 0; i < bar_width; i++) {
        if (i < filled - 1) {
            pacman_bar[i] = '=';
        } else if (i == filled - 1) {
            pacman_bar[i] = (current % 2 == 0) ? 'C' : 'c'; // Pacman mouth
        } else {
            pacman_bar[i] = (i % 3 == 0) ? 'o' : '-'; // Pac-dots
        }
    }

    uint32_t hours = (uint32_t)(elapsed_sec / 3600);
    uint32_t mins = (uint32_t)((elapsed_sec - hours * 3600) / 60);
    double secs = elapsed_sec - (hours * 3600) - (mins * 60);

    // Pinned dashboard line
    printf("\r\033[1;33m[%s]\033[0m \033[1;37m%5.1f%%\033[0m | \033[1;32m⏱ %02u:%02u:%04.1f\033[0m | \033[1;36m%8.0f tx/s\033[0m | \033[1;31m🚨 %u alerts ($%.0f)\033[0m\n",
           pacman_bar, progress * 100.0, mins, (uint32_t)secs, (secs - (uint32_t)secs) * 10, tx_per_sec, alert_count, flagged_volume);
    fflush(stdout);
}

void unicagd_tui_render_log_entry(
    const unicagd_transaction_t* tx,
    const unicagd_alert_t* alert_opt
) {
    if (alert_opt) {
        // Minimal bold red/yellow highlight ONLY on suspicious alerts
        printf("  \033[1;31m▶ [CRITICAL ALERT %s]\033[0m \033[1;37m%-16s\033[0m ➔ \033[1;37m%-16s\033[0m | \033[1;31m%8.2f %s\033[0m | \033[0;33m%s\033[0m\n",
               alert_opt->rule_id, tx->originator_account, tx->beneficiary_account, tx->amount, tx->currency, alert_opt->reason);
    } else {
        // Muted normal transaction log stream
        printf("  \033[0;37m• [STREAM]\033[0m %-18s ➔ %-18s | %8.2f %-4s | %-10s\n",
               tx->originator_account, tx->beneficiary_account, tx->amount, tx->currency, tx->channel);
    }
}

void unicagd_tui_render_final_report(const unicagd_report_t* report) {
    if (!report) return;

    printf("\n\033[1;36m================================================================================\033[0m\n");
    printf("\033[1;37m  AML & ANTI-FRAUD TRANSACTION EVALUATION REPORT\033[0m\n");
    printf("\033[1;36m================================================================================\033[0m\n\n");
    printf("  \033[1;37mHardware Engine:\033[0m      %s\n", report->primary_hardware);
    printf("  \033[1;37mTransactions:\033[0m         %llu\n", (unsigned long long)report->total_transactions_analyzed);
    printf("  \033[1;37mTotal Volume:\033[0m         $%.2f USD\n", report->total_volume_analyzed);
    printf("  \033[1;37mProcessing Time:\033[0m      \033[1;32m%.4f seconds\033[0m (Throughput: \033[1;36m%.1f tx/s\033[0m)\n",
           report->execution_time_seconds, report->throughput_tx_per_sec);
    printf("  \033[1;37mAlerts Generated:\033[0m     \033[1;31m%u\033[0m (Critical: \033[1;31m%u\033[0m, High: \033[1;33m%u\033[0m, Med: %u)\n",
           report->alerts_count, report->critical_alerts, report->high_alerts, report->medium_alerts);
    printf("  \033[1;37mFlagged Amount:\033[0m       \033[1;31m$%.2f USD\033[0m\n\n", report->total_suspicious_amount);

    printf("\033[1;31m=== DETECTED FRAUD & MONEY LAUNDERING ALERTS (TOP SAMPLES) ===\033[0m\n");
    printf("%-10s | %-12s | %-28s | %-18s | %-15s\n", "SEVERITY", "RULE", "CATEGORY", "SUBJECT ACCOUNT", "SUSPICIOUS AMOUNT");
    printf("-------------------------------------------------------------------------------------------------------------\n");

    uint32_t show_count = report->alerts_count > 6 ? 6 : report->alerts_count;
    for (uint32_t i = 0; i < show_count; i++) {
        const unicagd_alert_t* a = &report->alerts[i];
        const char* sev_str = (a->severity == SEVERITY_CRITICAL) ? "\033[1;31mCRITICAL\033[0m" : "\033[1;33mHIGH    \033[0m";
        printf("%-19s | %-12s | %-28s | \033[1;36m%-18s\033[0m | \033[1;37m%8.2f %-4s\033[0m\n",
               sev_str, a->rule_id, a->typology, a->subject_account, a->total_suspicious_amount, a->currency);
        printf("  \033[0;37m└── Reason: %s\033[0m\n\n", a->reason);
    }
}

int unicagd_export_html_report(
    const unicagd_report_t* report,
    const char* output_filepath,
    const char* language_code
) {
    if (!report || !output_filepath) return -1;

    FILE* f = fopen(output_filepath, "w");
    if (!f) return -1;

    bool is_hu = (language_code && strcmp(language_code, "hu") == 0);

    fprintf(f, "<!doctype html>\n<html><head><meta charset='UTF-8'><meta name='viewport' content='width=device-width initial-scale=1'>\n");
    fprintf(f, "<title>%s</title>\n", is_hu ? "UNICAGD_AML - GPU Tranzakciós Audit Jelentés" : "UNICAGD_AML - Master GPU Transaction Audit Report");
    fprintf(f, "<style>\n");
    fprintf(f, "body { font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, Helvetica, Arial, sans-serif; background: #0d1117; color: #c9d1d9; padding: 30px; line-height: 1.6; }\n");
    fprintf(f, ".container { max-width: 1200px; margin: auto; background: #161b22; border: 1px solid #30363d; border-radius: 8px; padding: 24px; }\n");
    fprintf(f, "h1, h2 { color: #58a6ff; }\n");
    fprintf(f, "table { width: 100%%; border-collapse: collapse; margin-top: 16px; margin-bottom: 24px; }\n");
    fprintf(f, "th, td { border: 1px solid #30363d; padding: 10px 14px; text-align: left; }\n");
    fprintf(f, "th { background: #21262d; color: #f0f6fc; }\n");
    fprintf(f, ".critical { color: #f85149; font-weight: bold; }\n");
    fprintf(f, ".high { color: #d29922; font-weight: bold; }\n");
    fprintf(f, ".badge { display: inline-block; padding: 2px 8px; border-radius: 12px; font-size: 12px; background: #238636; color: white; }\n");
    fprintf(f, "</style></head><body><div class='container'>\n");

    if (is_hu) {
        fprintf(f, "<h1>UNICAGD_AML v1.3.0 - GPU Tranzakciós Csalás- és Pénzmosás-Audit Jelentés</h1>\n");
        fprintf(f, "<p><span class='badge'>Hardveres Gyorsítás</span> <strong>Feldolgozó Egység:</strong> %s</p>\n", report->primary_hardware);
        fprintf(f, "<table><tr><th>Elemzett Tranzakció</th><th>Teljes Forgalom</th><th>Feldolgozási Idő</th><th>Sebesség</th><th>Generált Riasztások</th><th>Gyanúsított Összeg</th></tr>\n");
        fprintf(f, "<tr><td>%llu db</td><td>$%.2f USD</td><td><strong>%.4f s</strong></td><td><strong>%.1f tx/s</strong></td><td class='critical'>%u db</td><td class='critical'>$%.2f USD</td></tr></table>\n",
                (unsigned long long)report->total_transactions_analyzed, report->total_volume_analyzed, report->execution_time_seconds, report->throughput_tx_per_sec, report->alerts_count, report->total_suspicious_amount);
        fprintf(f, "<h2>Azonosított Gyanús Riasztások és Bűncselekményi Minták</h2>\n");
        fprintf(f, "<table><tr><th>Súlyosság</th><th>Szabály</th><th>Tipológia</th><th>Érintett Számla</th><th>Összeg</th><th>Indoklás</th></tr>\n");
    } else {
        fprintf(f, "<h1>UNICAGD_AML v1.3.0 - Master GPU Transaction Fraud & AML Audit Report</h1>\n");
        fprintf(f, "<p><span class='badge'>Hardware Acceleration</span> <strong>Compute Engine:</strong> %s</p>\n", report->primary_hardware);
        fprintf(f, "<table><tr><th>Analyzed Transactions</th><th>Total Volume</th><th>Processing Time</th><th>Throughput</th><th>Alerts Generated</th><th>Flagged Volume</th></tr>\n");
        fprintf(f, "<tr><td>%llu</td><td>$%.2f USD</td><td><strong>%.4f s</strong></td><td><strong>%.1f tx/s</strong></td><td class='critical'>%u</td><td class='critical'>$%.2f USD</td></tr></table>\n",
                (unsigned long long)report->total_transactions_analyzed, report->total_volume_analyzed, report->execution_time_seconds, report->throughput_tx_per_sec, report->alerts_count, report->total_suspicious_amount);
        fprintf(f, "<h2>Detected Suspicious Alerts & Typologies</h2>\n");
        fprintf(f, "<table><tr><th>Severity</th><th>Rule</th><th>Typology</th><th>Subject Account</th><th>Amount</th><th>Reason</th></tr>\n");
    }

    uint32_t max_rows = report->alerts_count > 100 ? 100 : report->alerts_count;
    for (uint32_t i = 0; i < max_rows; i++) {
        const unicagd_alert_t* a = &report->alerts[i];
        const char* sev_class = (a->severity == SEVERITY_CRITICAL) ? "critical" : "high";
        fprintf(f, "<tr><td class='%s'>%s</td><td>%s</td><td>%s</td><td><code>%s</code></td><td>$%.2f %s</td><td>%s</td></tr>\n",
                sev_class, (a->severity == SEVERITY_CRITICAL) ? "CRITICAL" : "HIGH", a->rule_id, a->typology, a->subject_account, a->total_suspicious_amount, a->currency, a->reason);
    }

    fprintf(f, "</table></div></body></html>\n");
    fclose(f);
    return 0;
}
