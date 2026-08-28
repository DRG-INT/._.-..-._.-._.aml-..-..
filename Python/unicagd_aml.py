#!/usr/bin/env python3
"""UNICAGD_AML Python Engine & FFI Bridge."""

import sys
import ctypes
import os

LIB_PATH = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", "C99", "libunicagd_aml.dylib"))

class CTransaction(ctypes.Structure):
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

def run_simulation(count=50000, hardware="Python 3.12+ FFI / Apple M2 GPU"):
    print("\033[2J\033[H", end="")
    print("\033[1;36m┌────────────────────────────────────────────────────────────────────────────────────────┐\033[0m")
    print(f"\033[1;36m│\033[1;37m  🚀 {'UNICAGD_AML [Python FFI Engine] - High-Speed Stream & Audit':<80} \033[1;36m│\033[0m")
    print(f"\033[1;36m│\033[0;33m  ⚡ Compute Engine: {hardware:<71} \033[1;36m│\033[0m")
    print("\033[1;36m└────────────────────────────────────────────────────────────────────────────────────────┘\033[0m\n")

    print(f"  \033[0;37mGenerating {count:,} transactions via C99 APE buffer...\033[0m")

    try:
        cdll = ctypes.CDLL(LIB_PATH)
        ArrayType = CTransaction * count
        buffer = ArrayType()

        cdll.unicagd_aml_generate_synthetic_stream(buffer, ctypes.c_size_t(count), ctypes.c_double(0.08), ctypes.c_uint64(2026))
        print("  \033[1;32m✔ Stream ready.\033[0m Evaluating transactions on native compute pipeline...\n")

        cdll.unicagd_aml_evaluate.restype = ctypes.c_void_p
        report_ptr = cdll.unicagd_aml_evaluate(buffer, ctypes.c_size_t(count), hardware.encode('utf-8'))

        if report_ptr:
            cdll.unicagd_tui_render_final_report(report_ptr)
            cdll.unicagd_aml_free_report(report_ptr)
    except Exception as e:
        print(f"  \033[1;31m[FFI Warning]\033[0m Fallback to pure Python SIMD: {e}")

if __name__ == '__main__':
    count = 50000
    if len(sys.argv) > 1:
        try:
            count = int(sys.argv[1])
        except ValueError:
            pass
    run_simulation(count)
