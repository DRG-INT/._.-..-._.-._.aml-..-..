use std::ffi::CString;
use unicagd_aml_rust::*;

/*
 * Entry point for Rust safe FFI engine.
 * Belépési pont a Rust safe FFI motorhoz.
 */
fn main() {
    /* Parse command-line arguments / Parancssori argumentumok feldolgozása */
    let args: Vec<String> = std::env::args().collect();
    /* Default transaction count / Alapértelmezett tranzakciószám */
    let count: usize = if args.len() > 1 {
        args[1].parse().unwrap_or(50000)
    } else {
        50000
    };

    /* Banner title / Banner cím */
    let title = CString::new("UNICAGD_AML [Rust Safe Engine] - High-Speed Stream & Audit").unwrap();
    /* Hardware description / Hardver leírás */
    let hardware = CString::new("Rust 1.75+ Safe FFI / Apple M2 Metal GPU").unwrap();

    /* Render banner / Banner megjelenítése */
    unsafe {
        unicagd_tui_render_banner(title.as_ptr(), hardware.as_ptr());
    }

    println!(
        "  \x1b[0;37m[Rust]\x1b[0m Allocating {} transaction structs on heap...",
        count
    );
    /* Allocate transaction buffer / Tranzakció puffer foglalása */
    let mut buffer: Vec<CTransaction> = vec![CTransaction::default(); count];
    /* Generate synthetic transaction stream / Szintetikus tranzakciófolyam generálása */
    unsafe {
        unicagd_aml_generate_synthetic_stream(buffer.as_mut_ptr(), count, 0.08, 2026);
    }

    println!("  \x1b[1;32m[Rust]\x1b[0m Evaluating transactions on native SIMD/GPU pipeline...\n");

    /* Evaluate transactions / Tranzakciók értékelése */
    unsafe {
        let report = unicagd_aml_evaluate(buffer.as_ptr(), count, hardware.as_ptr());
        if !report.is_null() {
            /* Render final report / Végső jelentés megjelenítése */
            unicagd_tui_render_final_report(report);
            /* Free report memory / Jelentés memóriafelszabadítása */
            unicagd_aml_free_report(report);
        }
    }
}
