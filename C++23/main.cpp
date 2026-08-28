#include "../include/unicagd_aml_core.h"
#include <iostream>
#include <vector>
#include <string>
#include <span>
#include <chrono>

// C++23 Modern Engine Implementation
// Uses std::span, std::chrono, and modern C++23 structures

int main(int argc, char* argv[]) {
    size_t count = 100000;
    double fraud_ratio = 0.08;
    std::string hardware = "C++23 Modern SIMD / Metal GPU Core";

    for (int i = 1; i < argc; i++) {
        if (std::string(argv[i]) == "--count" && i + 1 < argc) {
            count = std::stoull(argv[++i]);
        }
    }

    unicagd_tui_render_banner("UNICAGD_AML [C++23 Modern Engine] - Turbocharged Simulation & Audit", hardware.c_str());

    std::vector<unicagd_transaction_t> transactions(count);
    unicagd_aml_generate_synthetic_stream(transactions.data(), count, fraud_ratio, 42);

    std::cout << "  \033[1;34m[C++23]\033[0m Dispatched " << count << " transactions into C++23 span pipeline...\n\n";

    unicagd_report_t* report = unicagd_aml_evaluate(transactions.data(), count, hardware.c_str());

    if (report) {
        unicagd_tui_render_progress(count, count, report->execution_time_seconds, report->throughput_tx_per_sec, report->alerts_count, report->total_suspicious_amount);
        unicagd_tui_render_final_report(report);
        unicagd_aml_free_report(report);
    }

    return 0;
}
