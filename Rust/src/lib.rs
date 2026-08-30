use std::ffi::c_void;
use std::os::raw::{c_char, c_double};

/*
 * C-compatible transaction structure for FFI.
 * C-kompatibilis tranzakció struktúra FFI-hez.
 *
 * This struct mirrors the C99 unicagd_transaction_t layout exactly.
 * Ez a struktúra pontosan megfelel a C99 unicagd_transaction_t elrendezésnek.
 */
#[repr(C)]
#[derive(Clone, Copy)]
pub struct CTransaction {
    pub transaction_id: [u8; 64],        /* Unique transaction ID / Egyedi tranzakció azonosító */
    pub originator_account: [u8; 64],    /* Sender account / Küldő számla */
    pub beneficiary_account: [u8; 64],   /* Receiver account / Címzett számla */
    pub amount: c_double,                /* Transaction amount / Tranzakció összege */
    pub currency: [u8; 8],               /* Currency code / Pénznem kód */
    pub timestamp_ms: u64,               /* Timestamp in milliseconds / Időbélyeg milliszekundumban */
    pub ip_address: [u8; 32],            /* IP address / IP cím */
    pub device_id: [u8; 32],             /* Device ID / Eszköz azonosító */
    pub lat: c_double,                   /* Latitude / Szélesség */
    pub lon: c_double,                   /* Longitude / Hosszúság */
    pub country: [u8; 8],                /* Country code / Ország kód */
    pub channel: [u8; 16],               /* Transaction channel / Tranzakció csatorna */
    pub is_card_transaction: bool,       /* Is card transaction / Kártya tranzakció-e */
}

/*
 * Default implementation for CTransaction.
 * CTransaction alapértelmezett implementáció.
 */
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

/*
 * External C function declarations for FFI.
 * Külső C függvénydeklarációk FFI-hez.
 */
extern "C" {
    /*
     * Generate synthetic transaction stream.
     * Szintetikus tranzakciófolyam generálása.
     *
     * @param buffer Output transaction buffer / Kimeneti tranzakció puffer.
     * @param capacity Buffer capacity / Puffer kapacitás.
     * @param fraud_ratio Ratio of fraudulent transactions / Csalási tranzakciók aránya.
     * @param seed Random seed seed / Random seed érték.
     * @return Number of generated transactions / Generált tranzakciók száma.
     */
    pub fn unicagd_aml_generate_synthetic_stream(
        buffer: *mut CTransaction,
        capacity: usize,
        fraud_ratio: c_double,
        seed: u64,
    ) -> usize;

    /*
     * Evaluate transactions for AML/fraud patterns.
     * Tranzakciók értékelése AML/csalási minták szempontjából.
     *
     * @param transactions Array of transactions / Tranzakciók tömbje.
     * @param count Number of transactions / Tranzakciók száma.
     * @param hardware_target Hardware description / Hardver leírás.
     * @return Pointer to report structure / Jelentés struktúrára mutató pointer.
     */
    pub fn unicagd_aml_evaluate(
        transactions: *const CTransaction,
        count: usize,
        hardware_target: *const c_char,
    ) -> *mut c_void;

    /* Render banner / Banner megjelenítése */
    pub fn unicagd_tui_render_banner(title: *const c_char, hardware_info: *const c_char);
    /* Render final report / Végső jelentés megjelenítése */
    pub fn unicagd_tui_render_final_report(report: *const c_void);
    /* Free report memory / Jelentés memóriafelszabadítása */
    pub fn unicagd_aml_free_report(report: *mut c_void);
}
