module UnicagdAml

"""
UNICAGD_AML Julia Native ccall Interface Module.
UNICAGD_AML Julia Natív ccall Interface Modul.

Provides safest native C binding for AML engine via Julia ccall.
Biztonságos natív C kötést biztosít az AML motorhoz Julia ccall-n keresztül.
"""
export evaluate_transactions, generate_stream, run_tui_simulation

"""
Path to native C99 shared library.
Natív C99 megosztott könyvtár elérési útja.
"""
const LIB_PATH = normpath(joinpath(@__DIR__, "..", "C99", "libunicagd_aml.dylib"))

# =============================================================================
# Safest Native C Binding Layout in Julia
# Legbiztosabb Natív C Kötési Elrendezés Julia-ban
# =============================================================================

"""
C-compatible transaction structure.
C-kompatibilis tranzakció struktúra.
"""
struct CTransaction
    transaction_id::NTuple{64, UInt8}       /* Unique transaction ID / Egyedi tranzakció azonosító */
    originator_account::NTuple{64, UInt8}   /* Sender account / Küldő számla */
    beneficiary_account::NTuple{64, UInt8}  /* Receiver account / Címzett számla */
    amount::Float64                          /* Transaction amount / Tranzakció összege */
    currency::NTuple{8, UInt8}              /* Currency code / Pénznem kód */
    timestamp_ms::UInt64                     /* Timestamp in ms / Időbélyeg ms-ben */
    ip_address::NTuple{32, UInt8}           /* IP address / IP cím */
    device_id::NTuple{32, UInt8}            /* Device ID / Eszköz azonosító */
    lat::Float64                             /* Latitude / Szélesség */
    lon::Float64                             /* Longitude / Hosszúság */
    country::NTuple{8, UInt8}               /* Country code / Ország kód */
    channel::NTuple{16, UInt8}              /* Transaction channel / Tranzakció csatorna */
    is_card_transaction::Bool                /* Is card transaction / Kártya tranzakció-e */
end

"""
Generate synthetic transaction stream via native C call.
Szintetikus tranzakciófolyam generálása natív C hívással.

@param count Number of transactions to generate / Generálandó tranzakciók száma.
@param fraud_ratio Ratio of fraudulent transactions / Csalási tranzakciók aránya.
@return Buffer of generated transactions / Generált tranzakciók pufferje.
"""
function generate_stream(count::Int, fraud_ratio::Float64 = 0.08)
    buffer = Vector{CTransaction}(undef, count)
    ccall(
        (:unicagd_aml_generate_synthetic_stream, LIB_PATH),
        Csize_t,
        (Ptr{CTransaction}, Csize_t, Float64, UInt64),
        buffer, count, fraud_ratio, 9999
    )
    return buffer
end

"""
Run complete TUI simulation in Julia.
Teljes TUI szimuláció futtatása Julia-ban.

@param count Number of transactions / Tranzakciók száma.
"""
function run_tui_simulation(count::Int = 50000)
    println("\033[1;35m┌────────────────────────────────────────────────────────────────────────┐\033[0m")
    println("\033[1;35m│ 🚀 UNICAGD_AML [Julia Native ccall Interface] - High-Speed Audit Engine │\033[0m")
    println("\033[1;35m└────────────────────────────────────────────────────────────────────────┘\033[0m\n")

    println("  [Julia] Generating $(count) transactions via C99 APE core...")
    txs = generate_stream(count)

    println("  [Julia] Evaluating via safest C binding (ccall)...")
    hw_target = "Julia 1.10+ Native ccall / Metal GPU"

    report_ptr = ccall(
        (:unicagd_aml_evaluate, LIB_PATH),
        Ptr{Cvoid},
        (Ptr{CTransaction}, Csize_t, Cstring),
        txs, count, hw_target
    )

    if report_ptr != C_NULL
        /* Render final report / Végső jelentés megjelenítése */
        ccall((:unicagd_tui_render_final_report, LIB_PATH), Cvoid, (Ptr{Cvoid},), report_ptr)
        /* Free report memory / Jelentés memóriafelszabadítása */
        ccall((:unicagd_aml_free_report, LIB_PATH), Cvoid, (Ptr{Cvoid},), report_ptr)
    end
end

end # module
