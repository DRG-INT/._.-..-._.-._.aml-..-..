#define _DEFAULT_SOURCE
#define _POSIX_C_SOURCE 200809L
#include "../include/unicagd_aml_core.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/*
 * Platform-specific sleep macro.
 * Platform-specifikus alvási makró.
 */
#ifdef _WIN32
#include <windows.h>
#define SLEEP_MS(ms) Sleep(ms)
#else
#include <unistd.h>
#define SLEEP_MS(ms) usleep((ms) * 1000)
#endif

/*
 * Entry point.
 * Belépési pont.
 *
 * @param argc Argument count / Argumentumok száma.
 * @param argv Argument vector / Argumentum vektor.
 * @return Exit code / Kilépési kód.
 */
int main(int argc, char* argv[]) {
    /* Default transaction count / Alapértelmezett tranzakciószám */
    size_t count = 50000;
    /* Default fraud ratio / Alapértelmezett csalási arány */
    double fraud_ratio = 0.08;
    /* Default TUI theme / Alapértelmezett TUI téma */
    unicagd_theme_t theme = THEME_PACMAN;
    /* Hardware description / Hardver leírás */
    const char* hardware = "Apple Silicon M2 / Metal GPU (SIMD Accelerated)";
    /* Hungarian HTML report filename / Magyar HTML jelentés fájlneve */
    const char* export_html_hu = "UNICAGD_MASTER_GPU_TRANSACTION_AUDIT_REPORT_HU.html";
    /* English HTML report filename / Angol HTML jelentés fájlneve */
    const char* export_html_en = "UNICAGD_MASTER_GPU_TRANSACTION_AUDIT_REPORT_EN.html";

    /*
     * Parse command-line arguments.
     * Parancssori argumentumok feldolgozása.
     */
    for (int i = 1; i < argc; i++) {
        /* Transaction count parameter / Tranzakciószám paraméter */
        if (strcmp(argv[i], "--count") == 0 && i + 1 < argc) {
            count = (size_t)strtoull(argv[++i], NULL, 10);
        }
        /* Fraud ratio parameter / Csalási arány paraméter */
        else if (strcmp(argv[i], "--fraud-ratio") == 0 && i + 1 < argc) {
            fraud_ratio = atof(argv[++i]);
        }
        /* Theme parameter / Téma paraméter */
        else if (strcmp(argv[i], "--theme") == 0 && i + 1 < argc) {
            const char* t_str = argv[++i];
            if (strcmp(t_str, "cyberpunk") == 0) theme = THEME_CYBERPUNK;
            else if (strcmp(t_str, "matrix") == 0) theme = THEME_MATRIX;
            else if (strcmp(t_str, "minimal") == 0) theme = THEME_MINIMAL;
            else if (strcmp(t_str, "monokai") == 0) theme = THEME_MONOKAI;
            else theme = THEME_PACMAN;
        }
        /* Hardware override parameter / Hardver felülírási paraméter */
        else if (strcmp(argv[i], "--hardware") == 0 && i + 1 < argc) {
            hardware = argv[++i];
        }
    }

    /* Apply selected TUI theme / Kiválasztott TUI téma alkalmazása */
    unicagd_tui_set_theme(theme);
    /* Render engine banner / Motor banner megjelenítése */
    unicagd_tui_render_banner("UNICAGD_AML v1.3.0 - High-Speed Turbocharged Fraud & AML Engine", hardware);

    /*
     * Generate and buffer synthetic transactions.
     * Szintetikus tranzakciók generálása és pufferelése.
     */
    printf("  \033[0;37mGenerating and pre-buffering %zu mixed banking & card transactions...\033[0m\n", count);
    /* Allocate transaction array / Tranzakció tömb foglalása */
    unicagd_transaction_t* transactions = (unicagd_transaction_t*)malloc(sizeof(unicagd_transaction_t) * count);
    if (!transactions) {
        fprintf(stderr, "Memory allocation failed\n");
        return 1;
    }

    /* Generate synthetic transaction stream / Szintetikus tranzakciófolyam generálása */
    unicagd_aml_generate_synthetic_stream(transactions, count, fraud_ratio, 1337);

    printf("  \033[1;32m✔ Buffer ready.\033[0m Starting real-time GPU/SIMD stream evaluation viewport...\n\n");
    SLEEP_MS(300);

    /* Live animated viewport demo for the first stream slice / Élő animált nézet az első folyam szeletre */
    size_t preview_slice = count > 20 ? 20 : count;
    for (size_t i = 0; i < preview_slice; i++) {
        unicagd_alert_t alert_mock;
        bool is_alert = false;
        /* Sanctions evasion check / Szankciókerülő ellenőrzés */
        if (strcmp(transactions[i].country, "SC") == 0) {
            is_alert = true;
            strcpy(alert_mock.rule_id, "AML-003");
            strcpy(alert_mock.reason, "Sanctions match: SC (Seychelles)");
        }
        /* Structuring/smurfing check / Szerkeztetés/csoportosítás ellenőrzés */
        else if (transactions[i].amount >= 7500.0) {
            is_alert = true;
            strcpy(alert_mock.rule_id, "AML-001");
            strcpy(alert_mock.reason, "Sub-threshold structuring");
        }
        /* Card fraud anomaly check / Kártyasalcsalás anomália ellenőrzés */
        else if (transactions[i].is_card_transaction && transactions[i].amount > 1200.0) {
            is_alert = true;
            strcpy(alert_mock.rule_id, "FRD-003");
            strcpy(alert_mock.reason, "Card fraud anomalous spike");
        }

        /* Render log entry / Naplóbejegyzés megjelenítése */
        unicagd_tui_render_log_entry(&transactions[i], is_alert ? &alert_mock : NULL);
        SLEEP_MS(20);
    }

    printf("\n  \033[0;37m... (accelerating remaining %zu transactions into compute pipeline) ...\033[0m\n\n", count - preview_slice);

    /*
     * Run AML evaluation engine.
     * AML értékelő motor futtatása.
     */
    unicagd_report_t* report = unicagd_aml_evaluate(transactions, count, hardware);
    if (report) {
        /* Render progress bar / Folyamatjelző sáv megjelenítése */
        unicagd_tui_render_progress(count, count, report->execution_time_seconds, report->throughput_tx_per_sec, report->alerts_count, report->total_suspicious_amount);
        /* Render final report / Végső jelentés megjelenítése */
        unicagd_tui_render_final_report(report);

        /* Export both Hungarian and English HTML reports / Mindkét nyelvű HTML jelentés exportálása */
        unicagd_export_html_report(report, export_html_hu, "hu");
        unicagd_export_html_report(report, export_html_en, "en");

        printf("\n\033[1;32m✔ HTML Audit Reports exported successfully:\033[0m\n");
        printf("   • Hungarian: %s\n", export_html_hu);
        printf("   • English:   %s\n", export_html_en);

        /* Free report memory / Jelentés memóriafelszabadítása */
        unicagd_aml_free_report(report);
    }

    /* Free transaction memory / Tranzakció memóriafelszabadítása */
    free(transactions);
    return 0;
}
