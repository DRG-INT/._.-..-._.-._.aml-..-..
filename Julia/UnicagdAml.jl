module UnicagdAml

export evaluate_transactions, generate_stream, run_tui_simulation

const LIB_PATH = normpath(joinpath(@__DIR__, "..", "C99", "libunicagd_aml.dylib"))

# Safest native C binding layout in Julia
struct CTransaction
    transaction_id::NTuple{64, UInt8}
    originator_account::NTuple{64, UInt8}
    beneficiary_account::NTuple{64, UInt8}
    amount::Float64
    currency::NTuple{8, UInt8}
    timestamp_ms::UInt64
    ip_address::NTuple{32, UInt8}
    device_id::NTuple{32, UInt8}
    lat::Float64
    lon::Float64
    country::NTuple{8, UInt8}
    channel::NTuple{16, UInt8}
    is_card_transaction::Bool
end

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
        ccall((:unicagd_tui_render_final_report, LIB_PATH), Cvoid, (Ptr{Cvoid},), report_ptr)
        ccall((:unicagd_aml_free_report, LIB_PATH), Cvoid, (Ptr{Cvoid},), report_ptr)
    end
end

end # module
