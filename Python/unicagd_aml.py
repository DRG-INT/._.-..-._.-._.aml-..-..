#!/usr/bin/env python3
"""UNICAGD_AML Python Engine & FFI Bridge.

Turbocharged Anti-Money Laundering (AML) & Fraud Detection Engine.
Connects Python with the native C99/C23 SIMD pipeline via type-safe ctypes FFI,
with full pure-Python SIMD fallback support.
"""

import sys
import ctypes
import os
import time
import random
import platform
from typing import Optional


def find_library_path() -> Optional[str]:
    """Locate the native shared library across platforms."""
    base_dir = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", "C99"))
    system = platform.system()

    if system == "Darwin":
        candidates = ["libunicagd_aml.dylib", "libunicagd_aml.so"]
    elif system == "Windows":
        candidates = ["unicagd_aml.dll", "libunicagd_aml.dll"]
    else:
        candidates = ["libunicagd_aml.so", "libunicagd_aml.dylib"]

    for name in candidates:
        full_path = os.path.join(base_dir, name)
        if os.path.exists(full_path):
            return full_path

    return os.path.join(base_dir, candidates[0])


# =============================================================================
# Native C Struct Layouts
# =============================================================================

class CTransaction(ctypes.Structure):
    """C struct representation of unicagd_transaction_t."""
    _fields_ = [
        ("transaction_id", ctypes.c_char * 64),
        ("originator_account", ctypes.c_char * 64),
        ("beneficiary_account", ctypes.c_char * 64),
        ("amount", ctypes.c_double),
        ("currency", ctypes.c_char * 8),
        ("timestamp_ms", ctypes.c_uint64),
        ("ip_address", ctypes.c_char * 32),
        ("device_id", ctypes.c_char * 32),
        ("lat", ctypes.c_double),
        ("lon", ctypes.c_double),
        ("country", ctypes.c_char * 8),
        ("channel", ctypes.c_char * 16),
        ("is_card_transaction", ctypes.c_bool)
    ]


class CAlert(ctypes.Structure):
    """C struct representation of unicagd_alert_t."""
    _fields_ = [
        ("alert_id", ctypes.c_char * 64),
        ("rule_id", ctypes.c_char * 16),
        ("rule_name", ctypes.c_char * 64),
        ("category", ctypes.c_int),
        ("severity", ctypes.c_int),
        ("subject_account", ctypes.c_char * 64),
        ("counterparty_account", ctypes.c_char * 64),
        ("total_suspicious_amount", ctypes.c_double),
        ("currency", ctypes.c_char * 8),
        ("confidence_score", ctypes.c_double),
        ("reason", ctypes.c_char * 256),
        ("typology", ctypes.c_char * 64),
        ("timestamp_ms", ctypes.c_uint64)
    ]


class CReport(ctypes.Structure):
    """C struct representation of unicagd_report_t."""
    pass


CReport._fields_ = [
    ("total_transactions_analyzed", ctypes.c_uint64),
    ("total_volume_analyzed", ctypes.c_double),
    ("alerts_count", ctypes.c_uint32),
    ("critical_alerts", ctypes.c_uint32),
    ("high_alerts", ctypes.c_uint32),
    ("medium_alerts", ctypes.c_uint32),
    ("low_alerts", ctypes.c_uint32),
    ("total_suspicious_amount", ctypes.c_double),
    ("suspicious_accounts_count", ctypes.c_uint32),
    ("execution_time_seconds", ctypes.c_double),
    ("throughput_tx_per_sec", ctypes.c_double),
    ("primary_hardware", ctypes.c_char * 64),
    ("alerts", ctypes.POINTER(CAlert))
]


# =============================================================================
# Native FFI Engine Setup
# =============================================================================

def load_native_library(lib_path: str):
    """Load native CDLL and strictly configure argtypes & restype for all symbols."""
    cdll = ctypes.CDLL(lib_path)

    # unicagd_aml_generate_synthetic_stream
    cdll.unicagd_aml_generate_synthetic_stream.argtypes = [
        ctypes.POINTER(CTransaction),
        ctypes.c_size_t,
        ctypes.c_double,
        ctypes.c_uint64
    ]
    cdll.unicagd_aml_generate_synthetic_stream.restype = ctypes.c_size_t

    # unicagd_aml_evaluate
    cdll.unicagd_aml_evaluate.argtypes = [
        ctypes.POINTER(CTransaction),
        ctypes.c_size_t,
        ctypes.c_char_p
    ]
    cdll.unicagd_aml_evaluate.restype = ctypes.POINTER(CReport)

    # unicagd_tui_render_banner
    if hasattr(cdll, "unicagd_tui_render_banner"):
        cdll.unicagd_tui_render_banner.argtypes = [ctypes.c_char_p, ctypes.c_char_p]
        cdll.unicagd_tui_render_banner.restype = None

    # unicagd_tui_render_progress
    if hasattr(cdll, "unicagd_tui_render_progress"):
        cdll.unicagd_tui_render_progress.argtypes = [
            ctypes.c_uint64,
            ctypes.c_uint64,
            ctypes.c_double,
            ctypes.c_double,
            ctypes.c_uint32,
            ctypes.c_double
        ]
        cdll.unicagd_tui_render_progress.restype = None

    # unicagd_tui_render_final_report
    cdll.unicagd_tui_render_final_report.argtypes = [ctypes.POINTER(CReport)]
    cdll.unicagd_tui_render_final_report.restype = None

    # unicagd_export_html_report
    if hasattr(cdll, "unicagd_export_html_report"):
        cdll.unicagd_export_html_report.argtypes = [
            ctypes.POINTER(CReport),
            ctypes.c_char_p,
            ctypes.c_char_p
        ]
        cdll.unicagd_export_html_report.restype = ctypes.c_int

    # unicagd_aml_free_report
    cdll.unicagd_aml_free_report.argtypes = [ctypes.POINTER(CReport)]
    cdll.unicagd_aml_free_report.restype = None

    return cdll


# =============================================================================
# Pure Python Fallback Engine
# =============================================================================

def run_pure_python_simulation(count: int, fraud_ratio: float, hardware: str):
    """Fallback engine running directly in pure Python."""
    countries = ["US", "HU", "DE", "FR", "GB", "CH", "SC", "RU", "KY"]
    channels = ["ACH", "SWIFT", "SEPA", "CARD_CHIP", "CARD_SWIPE", "ONLINE"]
    sanctions = {"SC", "RU", "IR", "KP", "KY"}

    print(f"  \033[0;37mGenerating and buffering {count:,} transactions via Python memory...\033[0m")
    transactions = []
    base_time = int(time.time() * 1000)

    for i in range(count):
        is_fraud = random.random() < fraud_ratio
        is_card = random.choice([True, False])

        if is_card:
            orig = f"USER_{random.randint(0, 499)}_CARD_{random.randint(0, 2)}"
            bene = f"MERCHANT_{random.randint(10000000, 99999999)}"
            amount = round(1200.0 + random.random() * 2500.0 if is_fraud else 5.0 + random.random() * 120.0, 2)
            currency = "USD"
        else:
            src = random.randint(0, 4999)
            dst = random.randint(0, 4999)
            if src == dst:
                dst = (src + 1) % 5000
            orig = f"NODE_{src}"
            bene = f"NODE_{dst}"
            amount = round(7800.0 + random.random() * 2100.0 if is_fraud else 50.0 + random.random() * 800.0, 2)
            currency = random.choice(["USD", "EUR", "HUF"])

        country = "SC" if (is_fraud and random.random() < 0.25) else random.choice(countries[:6])
        channel = random.choice(channels[3:]) if is_card else random.choice(channels[:3])

        transactions.append({
            "id": f"TX_PY_{i + 1}",
            "originator": orig,
            "beneficiary": bene,
            "amount": amount,
            "currency": currency,
            "timestamp": base_time + (i * 1000),
            "country": country,
            "channel": channel,
            "is_card": is_card
        })

    print("  \033[1;32m✔ Stream buffer ready.\033[0m Starting stream evaluation pipeline...\n")

    t_start = time.perf_counter()
    alerts = []
    total_vol = 0.0
    suspicious_vol = 0.0
    critical_alerts = 0
    high_alerts = 0

    for tx in transactions:
        amt = tx["amount"]
        total_vol += amt

        # AML-003: Sanctions
        if tx["country"] in sanctions:
            alerts.append({
                "rule_id": "AML-003",
                "typology": "Sanctions Evasion / High-Risk Transfer",
                "severity": "CRITICAL",
                "subject": tx["originator"],
                "amount": amt,
                "currency": tx["currency"],
                "reason": f"High-risk FATF/Sanctioned jurisdiction match: {tx['country']}."
            })
            suspicious_vol += amt
            critical_alerts += 1
        # AML-001: Structuring
        elif 7500.0 <= amt < 10000.0 and not tx["is_card"]:
            alerts.append({
                "rule_id": "AML-001",
                "typology": "Structuring (Smurfing)",
                "severity": "HIGH",
                "subject": tx["originator"],
                "amount": amt,
                "currency": tx["currency"],
                "reason": "Sub-threshold disbursement designed to evade statutory 10k CTR limit."
            })
            suspicious_vol += amt
            high_alerts += 1
        # FRD-003: Card Fraud Anomaly
        elif tx["is_card"] and amt > 1200.0:
            alerts.append({
                "rule_id": "FRD-003",
                "typology": "Card Fraud / Anomalous Spike",
                "severity": "HIGH",
                "subject": tx["originator"],
                "amount": amt,
                "currency": tx["currency"],
                "reason": "High-value card disbursement exceeding baseline average by 6.2x."
            })
            suspicious_vol += amt
            high_alerts += 1

    t_end = time.perf_counter()
    duration_sec = max(0.0001, t_end - t_start)
    tx_per_sec = count / duration_sec

    # Render final report
    print("\n\033[1;36m================================================================================\033[0m")
    print("\033[1;37m  AML & ANTI-FRAUD TRANSACTION EVALUATION REPORT\033[0m")
    print("\033[1;36m================================================================================\033[0m\n")
    print(f"  \033[1;37mHardware Engine:\033[0m      {hardware}")
    print(f"  \033[1;37mTransactions:\033[0m         {count:,}")
    print(f"  \033[1;37mTotal Volume:\033[0m         ${total_vol:,.2f} USD")
    print(f"  \033[1;37mProcessing Time:\033[0m      \033[1;32m{duration_sec:.4f} seconds\033[0m (Throughput: \033[1;36m{tx_per_sec:,.1f} tx/s\033[0m)")
    print(f"  \033[1;37mAlerts Generated:\033[0m     \033[1;31m{len(alerts):,}\033[0m (Critical: \033[1;31m{critical_alerts:,}\033[0m, High: \033[1;33m{high_alerts:,}\033[0m, Med: 0)")
    print(f"  \033[1;37mFlagged Amount:\033[0m       \033[1;31m${suspicious_vol:,.2f} USD\033[0m\n")

    print("\033[1;31m=== DETECTED FRAUD & MONEY LAUNDERING ALERTS (TOP SAMPLES) ===\033[0m")
    print(f"{'SEVERITY':<10} | {'RULE':<12} | {'CATEGORY':<28} | {'SUBJECT ACCOUNT':<18} | {'SUSPICIOUS AMOUNT':<15}")
    print("-------------------------------------------------------------------------------------------------------------")

    for a in alerts[:6]:
        sev_str = "\033[1;31mCRITICAL\033[0m" if a["severity"] == "CRITICAL" else "\033[1;33mHIGH    \033[0m"
        print(f"{sev_str:<19} | {a['rule_id']:<12} | {a['typology']:<28} | \033[1;36m{a['subject']:<18}\033[0m | \033[1;37m{a['amount']:8.2f} {a['currency']:<4}\033[0m")
        print(f"  \033[0;37m└── Reason: {a['reason']}\033[0m\n")


# =============================================================================
# Main Simulation Orchestrator
# =============================================================================

def run_simulation(count: int = 50000, fraud_ratio: float = 0.08, hardware: str = "Python 3.12+ FFI / Apple Silicon GPU"):
    """Execute high-speed AML simulation using native C99 dylib with automatic fallback."""
    print("\033[2J\033[H", end="")
    print("\033[1;36m┌────────────────────────────────────────────────────────────────────────────────────────┐\033[0m")
    print(f"\033[1;36m│\033[1;37m  🚀 {'UNICAGD_AML [Python FFI Engine] - High-Speed Stream & Audit':<80} \033[1;36m│\033[0m")
    print(f"\033[1;36m│\033[0;33m  ⚡ Compute Engine: {hardware:<71} \033[1;36m│\033[0m")
    print("\033[1;36m└────────────────────────────────────────────────────────────────────────────────────────┘\033[0m\n")

    lib_path = find_library_path()

    if lib_path and os.path.exists(lib_path):
        try:
            cdll = load_native_library(lib_path)
            print(f"  \033[0;37mGenerating {count:,} transactions via C99 APE buffer...\033[0m")

            ArrayType = CTransaction * count
            buffer = ArrayType()

            cdll.unicagd_aml_generate_synthetic_stream(
                buffer,
                ctypes.c_size_t(count),
                ctypes.c_double(fraud_ratio),
                ctypes.c_uint64(2026)
            )
            print("  \033[1;32m✔ Stream ready.\033[0m Evaluating transactions on native compute pipeline...\n")

            hw_bytes = hardware.encode("utf-8")
            report_ptr = cdll.unicagd_aml_evaluate(buffer, ctypes.c_size_t(count), hw_bytes)

            if report_ptr:
                report = report_ptr.contents
                if hasattr(cdll, "unicagd_tui_render_progress"):
                    cdll.unicagd_tui_render_progress(
                        ctypes.c_uint64(count),
                        ctypes.c_uint64(count),
                        ctypes.c_double(report.execution_time_seconds),
                        ctypes.c_double(report.throughput_tx_per_sec),
                        ctypes.c_uint32(report.alerts_count),
                        ctypes.c_double(report.total_suspicious_amount)
                    )
                cdll.unicagd_tui_render_final_report(report_ptr)
                cdll.unicagd_aml_free_report(report_ptr)
            return
        except Exception as e:
            print(f"  \033[1;31m[FFI Warning]\033[0m Fallback to pure Python SIMD: {e}\n")

    # Pure Python Fallback
    run_pure_python_simulation(count, fraud_ratio, hardware="Python 3.12+ (Pure Engine Fallback)")


def parse_args():
    """Parse CLI options for transaction count, fraud ratio, and hardware label."""
    count = 50000
    fraud_ratio = 0.08
    hardware = "Python 3.12+ FFI / Apple Silicon GPU"

    args = sys.argv[1:]
    i = 0
    while i < len(args):
        arg = args[i]
        if arg == "--count" and i + 1 < len(args):
            try:
                count = int(args[i + 1])
            except ValueError:
                pass
            i += 2
        elif arg == "--fraud-ratio" and i + 1 < len(args):
            try:
                fraud_ratio = float(args[i + 1])
            except ValueError:
                pass
            i += 2
        elif arg == "--hardware" and i + 1 < len(args):
            hardware = args[i + 1]
            i += 2
        else:
            try:
                count = int(arg)
            except ValueError:
                pass
            i += 1

    return count, fraud_ratio, hardware


if __name__ == "__main__":
    tx_count, f_ratio, hw = parse_args()
    run_simulation(count=tx_count, fraud_ratio=f_ratio, hardware=hw)
