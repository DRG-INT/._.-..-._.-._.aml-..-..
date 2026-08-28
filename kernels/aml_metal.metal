#include <metal_stdlib>
using namespace metal;

// UNICAGD_AML - Metal GPU Compute Pipeline (Apple Silicon M1/M2/M3/M4 & Intel)
// High-Throughput Parallel Graph Topology, Cycle Reduction & Sanctions Evasion

struct MetalTransaction {
    uint32_t source_node;
    uint32_t target_node;
    float amount;
    uint32_t timestamp_step;
    uint32_t country_code; // ISO-3166 packed
    uint32_t is_card;
};

struct MetalAlertResult {
    uint32_t alert_type; // 0: None, 1: Smurfing, 2: PassThrough, 3: Cycle, 4: Sanction, 5: Card Fraud
    uint32_t subject_node;
    float suspicious_volume;
    float confidence;
};

kernel void aml_metal_hypergraph_kernel(
    device const MetalTransaction* transactions [[buffer(0)]],
    device MetalAlertResult* alert_out [[buffer(1)]],
    constant uint32_t& total_transactions [[buffer(2)]],
    uint32_t id [[thread_position_in_grid]])
{
    if (id >= total_transactions) return;

    MetalTransaction current = transactions[id];
    uint32_t src = current.source_node;
    uint32_t dst = current.target_node;

    alert_out[id].alert_type = 0;
    alert_out[id].subject_node = src;
    alert_out[id].suspicious_volume = 0.0f;
    alert_out[id].confidence = 0.0f;

    // 1. High-Risk Jurisdiction / Sanctions check (e.g. packed 'SC', 'RU', 'IR', 'KP')
    if (current.country_code == 0x5343 || current.country_code == 0x5255 || current.country_code == 0x4952) {
        alert_out[id].alert_type = 4; // Sanction
        alert_out[id].suspicious_volume = current.amount;
        alert_out[id].confidence = 0.98f;
        return;
    }

    if (src == dst) return;

    // 2. Parallel Circular Layering search
    for (uint32_t i = 0; i < total_transactions; i++) {
        if (transactions[i].source_node == dst && transactions[i].target_node == src) {
            alert_out[id].alert_type = 3; // Cycle Layering
            alert_out[id].suspicious_volume = current.amount;
            alert_out[id].confidence = 0.96f;
            break;
        }
    }
}
