use std::ffi::c_void;
use std::os::raw::{c_char, c_double};

#[repr(C)]
#[derive(Clone, Copy)]
pub struct CTransaction {
    pub transaction_id: [u8; 64],
    pub originator_account: [u8; 64],
    pub beneficiary_account: [u8; 64],
    pub amount: c_double,
    pub currency: [u8; 8],
    pub timestamp_ms: u64,
    pub ip_address: [u8; 32],
    pub device_id: [u8; 32],
    pub lat: c_double,
    pub lon: c_double,
    pub country: [u8; 8],
    pub channel: [u8; 16],
    pub is_card_transaction: bool,
}

impl Default for CTransaction {
    fn default() -> Self {
        Self {
            transaction_id: [0; 64],
            originator_account: [0; 64],
            beneficiary_account: [0; 64],
            amount: 0.0,
            currency: [0; 8],
            timestamp_ms: 0,
            ip_address: [0; 32],
            device_id: [0; 32],
            lat: 0.0,
            lon: 0.0,
            country: [0; 8],
            channel: [0; 16],
            is_card_transaction: false,
        }
    }
}

extern "C" {
    pub fn unicagd_aml_generate_synthetic_stream(
        buffer: *mut CTransaction,
        capacity: usize,
        fraud_ratio: c_double,
        seed: u64,
    ) -> usize;

    pub fn unicagd_aml_evaluate(
        transactions: *const CTransaction,
        count: usize,
        hardware_target: *const c_char,
    ) -> *mut c_void;

    pub fn unicagd_tui_render_banner(title: *const c_char, hardware_info: *const c_char);
    pub fn unicagd_tui_render_final_report(report: *const c_void);
    pub fn unicagd_aml_free_report(report: *mut c_void);
}
