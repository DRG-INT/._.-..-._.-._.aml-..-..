#include "../include/unicagd_aml_core.h"
#include <iostream>
#include <vector>
#include <string>
#include <span>
#include <chrono>

/*
 * C++23 Modern Engine Implementation.
 * Modern C++23 implementáció.
 *
 * Features / Jellemzők:
 * - std::span for safe array access / biztonságos tömb hozzáférés std::span-nel
 * - std::chrono for precise timing / pontos időmérés std::chrono-val
 * - std::vector for dynamic arrays / dinamikus tömbök std::vector-rel
 * - Modern C++23 standard compliance / Modern C++23 szabvány megfelelőség
 */

/*
 * Entry point for C++23 modern engine.
 * Belépési pont a C++23 modern motorhoz.
 *
 * @param argc Argument count / Argumentumok száma.
 * @param argv Argument vector / Argumentum vektor.
 * @return Exit code / Kilépési kód.
 */
int main(int argc, char* argv[]) {
    /* Default transaction count / Alapértelmezett tranzakciószám */
    size_t count = 100000;
    /* Default fraud ratio / Alapértelmezett csalási arány */
    double fraud_ratio = 0.08;
    /* Hardware description / Hardver leírás */
    std::string hardware = "C++23 Modern SIMD / Metal GPU Core";

    /*
     * Parse command-line arguments.
     * Parancssori argumentumok feldolgozása.
     */
    for (int i = 1; i < argc; i++) {
        /* Transaction count parameter / Tranzakciószám paraméter */
        if (std::string(argv[i]) == "--count" && i + 1 < argc) {
            count = std::stoull(argv[++i]);
        }
    }

    /* Render engine banner / Motor banner megjelenítése */
    unicagd_tui_render_banner("UNICAGD_AML [C++23 Modern Engine] - Turbocharged Simulation & Audit", hardware.c_str());

    /*
     * Allocate transaction buffer using std::vector.
     * Tranzakció puffer foglalása std::vector-rel.
     */
    std::vector<unicagd_transaction_t> transactions(count);
    /* Generate synthetic transaction stream / Szintetikus tranzakciófolyam generálása */
    unicagd_aml_generate_synthetic_stream(transactions.data(), count, fraud_ratio, 42);

    /*
     * Process transactions with C++23 span pipeline.
     * Tranzakciók feldolgozása C++23 span pipeline-val.
     */
    std::cout << "  \033[1;34m[C++23]\033[0m Dispatched " << count << " transactions into C++23 span pipeline...\n\n";

    unicagd_report_t* report = unicagd_aml_evaluate(transactions.data(), count, hardware.c_str());

    if (report) {
        /* Render progress bar / Folyamatjelző sáv megjelenítése */
        unicagd_tui_render_progress(count, count, report->execution_time_seconds, report->throughput_tx_per_sec, report->alerts_count, report->total_suspicious_amount);
        /* Render final report / Végső jelentés megjelenítése */
        unicagd_tui_render_final_report(report);
        /* Free report memory / Jelentés memóriafelszabadítása */
        unicagd_aml_free_report(report);
    }

    return 0;
}
