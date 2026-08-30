#include "../include/unicagd_aml_core.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * C23 Standard Implementation.
 * Modern C23 implementáció.
 *
 * Features / Jellemzők:
 * - type-safe constexpr constants / típusbiztos constexpr konstansok
 * - nullptr support / nullptr támogatás
 * - Modern standard compliance / Modern szabvány megfelelőség
 */

/*
 * Default transaction count for C23 engine.
 * Alapértelmezett tranzakciószám a C23 motorhoz.
 */
constexpr size_t DEFAULT_TRANSACTIONS = 75000;

/*
 * Default fraud ratio for C23 engine.
 * Alapértelmezett csalási arány a C23 motorhoz.
 */
constexpr double DEFAULT_FRAUD_RATIO = 0.08;

/*
 * Entry point for C23 modern engine.
 * Belépési pont a C23 modern motorhoz.
 *
 * @param argc Argument count / Argumentumok száma.
 * @param argv Argument vector / Argumentum vektor.
 * @return Exit code / Kilépési kód.
 */
int main(int argc, char* argv[]) {
    /* Default transaction count / Alapértelmezett tranzakciószám */
    size_t count = DEFAULT_TRANSACTIONS;
    /* Default fraud ratio / Alapértelmezett csalási arány */
    double fraud_ratio = DEFAULT_FRAUD_RATIO;
    /* Hardware description / Hardver leírás */
    const char* hardware = "C23 Modern SIMD / Metal GPU";

    /*
     * Parse command-line arguments.
     * Parancssori argumentumok feldolgozása.
     */
    for (int i = 1; i < argc; i++) {
        /* Transaction count parameter / Tranzakciószám paraméter */
        if (strcmp(argv[i], "--count") == 0 && i + 1 < argc) {
            count = (size_t)strtoull(argv[++i], nullptr, 10);
        }
    }

    /* Render engine banner / Motor banner megjelenítése */
    unicagd_tui_render_banner("UNICAGD_AML [C23 Modern Engine] - Turbocharged Simulation & Audit", hardware);

    /*
     * Allocate transaction buffer.
     * Tranzakció puffer foglalása.
     */
    unicagd_transaction_t* transactions = (unicagd_transaction_t*)calloc(count, sizeof(unicagd_transaction_t));
    if (transactions == nullptr) {
        fprintf(stderr, "C23 Allocation Error\n");
        return 1;
    }

    /* Generate synthetic transaction stream / Szintetikus tranzakciófolyam generálása */
    unicagd_aml_generate_synthetic_stream(transactions, count, fraud_ratio, 2026);

    /*
     * Process transactions with C23 pipeline.
     * Tranzakciók feldolgozása C23 pipeline-val.
     */
    printf("  \033[1;32m[C23]\033[0m Processing %zu transactions with modern C23 pipeline...\n\n", count);
    unicagd_report_t* report = unicagd_aml_evaluate(transactions, count, hardware);

    if (report != nullptr) {
        /* Render progress bar / Folyamatjelző sáv megjelenítése */
        unicagd_tui_render_progress(count, count, report->execution_time_seconds, report->throughput_tx_per_sec, report->alerts_count, report->total_suspicious_amount);
        /* Render final report / Végső jelentés megjelenítése */
        unicagd_tui_render_final_report(report);
        /* Free report memory / Jelentés memóriafelszabadítása */
        unicagd_aml_free_report(report);
    }

    /* Free transaction memory / Tranzakció memóriafelszabadítása */
    free(transactions);
    return 0;
}
