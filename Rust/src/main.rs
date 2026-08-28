use std::ffi::CString;
use unicagd_aml_rust::*;

fn main() {
    let args: Vec<String> = std::env::args().collect();
    let count: usize = if args.len() > 1 {
        args[1].parse().unwrap_or(50000)
    } else {
        50000
    };

    let title = CString::new("UNICAGD_AML [Rust Safe Engine] - High-Speed Stream & Audit").unwrap();
    let hardware = CString::new("Rust 1.75+ Safe FFI / Apple M2 Metal GPU").unwrap();

    unsafe {
        unicagd_tui_render_banner(title.as_ptr(), hardware.as_ptr());
    }

    println!(
        "  \x1b[0;37m[Rust]\x1b[0m Allocating {} transaction structs on heap...",
        count
    );
    let mut buffer: Vec<CTransaction> = vec![CTransaction::default(); count];
    unsafe {
        unicagd_aml_generate_synthetic_stream(buffer.as_mut_ptr(), count, 0.08, 2026);
    }

    println!("  \x1b[1;32m[Rust]\x1b[0m Evaluating transactions on native SIMD/GPU pipeline...\n");

    unsafe {
        let report = unicagd_aml_evaluate(buffer.as_ptr(), count, hardware.as_ptr());
        if !report.is_null() {
            unicagd_tui_render_final_report(report);
            unicagd_aml_free_report(report);
        }
    }
}
