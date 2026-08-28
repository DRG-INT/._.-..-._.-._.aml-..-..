#include "../include/unicagd_aml_core.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// C23 Standard Implementation
// Modern C23 type-safe and constexpr features

constexpr size_t DEFAULT_TRANSACTIONS = 75000;
constexpr double DEFAULT_FRAUD_RATIO = 0.08;

int main(int argc, char* argv[]) {
    size_t count = DEFAULT_TRANSACTIONS;
    double fraud_ratio = DEFAULT_FRAUD_RATIO;
    const char* hardware = "C23 Modern SIMD / Metal GPU";

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--count") == 0 && i + 1 < argc) {
            count = (size_t)strtoull(argv[++i], nullptr, 10);
        }
    }

    unicagd_tui_render_banner("UNICAGD_AML [C23 Modern Engine] - Turbocharged Simulation & Audit", hardware);

    unicagd_transaction_t* transactions = (unicagd_transaction_t*)calloc(count, sizeof(unicagd_transaction_t));
    if (transactions == nullptr) {
        fprintf(stderr, "C23 Allocation Error\n");
        return 1;
    }

    unicagd_aml_generate_synthetic_stream(transactions, count, fraud_ratio, 2026);

    printf("  \033[1;32m[C23]\033[0m Processing %zu transactions with modern C23 pipeline...\n\n", count);
    unicagd_report_t* report = unicagd_aml_evaluate(transactions, count, hardware);

    if (report != nullptr) {
        unicagd_tui_render_progress(count, count, report->execution_time_seconds, report->throughput_tx_per_sec, report->alerts_count, report->total_suspicious_amount);
        unicagd_tui_render_final_report(report);
        unicagd_aml_free_report(report);
    }

    free(transactions);
    return 0;
}
