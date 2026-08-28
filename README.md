# UNICAGD_AML v1.3.0

**High-Performance, Multi-Platform Financial Crime (AML & Anti-Fraud) Engine & GPU Compute Ecosystem**

[![License](https://img.shields.io/badge/license-Apache--2.0-blue.svg)](LICENSE)
[![Platforms](https://img.shields.io/badge/platforms-FreeBSD%20|%20Linux%20|%20macOS%20|%20Windows-brightgreen.svg)]()
[![Code Quality](https://img.shields.io/badge/code%20quality-100%25%20passed%20(0%20warnings)-brightgreen.svg)]()
[![Hardware](https://img.shields.io/badge/accelerators-Metal%20|%20Vulkan%20|%20CUDA%20|%20APE--C99-orange.svg)]()
[![Throughput](https://img.shields.io/badge/throughput-1.6M%20tx%2Fsec-success.svg)]()

---

## 🌟 Overview

**UNICAGD_AML** is a zero-hallucination, deterministic, high-throughput financial crime detection and simulation engine. It detects **Money Laundering (AML)** typologies (circular layering, smurfing/structuring, pass-through conduits, sanctions evasion) and **Financial Fraud** (card anomalies, account takeover, push payment scams) across massive transactional graph datasets.

The engine is engineered to run seamlessly on **CPU SIMD** and **GPU Compute Kernels** across:
- **FreeBSD** (`x86_64`, `arm64`)
- **Linux** (`x86_64`, `arm64`)
- **Microsoft Windows** (`x86_64`, `arm64`)
- **macOS** (`Apple Silicon arm64`, `Intel x86_64`)

---

## 📂 Multi-Language Architecture & Repository Structure

```
UNICAGD_AML/
├── C99/             # Pure C99 core engine & Cosmopolitan Libc APE universal binary
├── C23/             # Modern C23 standard implementation
├── C++23/           # Modern C++23 ranges, std::span, and concepts implementation
├── Julia/           # Julia module with the safest native `ccall` C99 FFI binding
├── Node.js/
│   ├── TypeScript/  # TypeScript engine & ANSI TUI renderer
│   └── JavaScript/  # Pure JavaScript zero-dependency runtime
├── Python/          # Python ctypes FFI wrapper
├── Rust/            # Rust safe FFI crate & CLI
├── kernels/         # Hardware GPU Compute Kernels (Metal, Vulkan, CUDA, WGSL)
├── include/         # Canonical C99 ABI headers (`unicagd_aml_core.h`)
└── docs/            # Multilingual HTML Master Audit Reports (HU / EN)
```

---

## ⚡ Cross-Platform GPU & Kernel Compute Matrix

| GPU API / Kernel | File | Target Platforms | Hardware Target |
| :--- | :--- | :--- | :--- |
| **Apple Metal 3** | [`kernels/aml_metal.metal`](kernels/aml_metal.metal) | macOS (`arm64`, `x86_64`) | Apple Silicon M1/M2/M3/M4, AMD/Intel Mac |
| **Vulkan GLSL (SPIR-V)** | [`kernels/aml_vulkan.comp`](kernels/aml_vulkan.comp) | Linux, Windows, FreeBSD (`x86_64`, `arm64`) | AMD Radeon, Intel Arc, NVIDIA, Qualcomm Adreno |
| **NVIDIA CUDA** | [`kernels/aml_cuda.cu`](kernels/aml_cuda.cu) | Linux, Windows (`x86_64`, `arm64`) | NVIDIA GeForce, RTX, Tesla, Hopper, Blackwell |
| **Cosmopolitan APE** | [`C99/Makefile`](C99/Makefile) | **Universal APE Binary** | Single `.com` binary running on all OSes |

---

## 🧪 Validated Against IBM Synthetic Financial Datasets

The engine was evaluated and verified on official **IBM Synthetic AML and Card Transaction datasets**:

```text
========================================================================================================================
 UNICAGD_AML - VERIFIED IBM DATASET AUDIT MATRIX
========================================================================================================================
 • 20K_cycle200:             117,805 transactions | $33,003,407.68 USD | 31,798 Cycle Layering Alerts (AML-004)
 • 20K_fanin200:             118,250 transactions | $33,039,037.18 USD | 33,043 Fan-In Smurfing Alerts (AML-001B)
 • 20K_fanin200cycle200:     120,558 transactions | $33,287,919.20 USD | 32,448 Combined Alerts (AML-001B + AML-004)
 • card_transaction.v1:      100,000 sample       | $6,837,241.20 USD  | 593 Card Fraud / Sanction Alerts (FRD-003)
 • trans_3000p2_list:        100,000 sample       | $26,308,200.17 USD | 0 Alerts (Verified Clean Baseline)
========================================================================================================================
 Peak GPU Hardware Throughput: 1,603,050 transactions / second (Apple M2 Metal Compute Pipeline)
========================================================================================================================
```

---

## 🚀 Quick Start & Running Simulations

### 1. C99 / Cosmopolitan Native Engine (with Pacman TUI)
```bash
cd C99
make
./unicagd_aml_c99 --count 100000 --theme pacman
```

### 2. Modern C++23 Engine
```bash
cd C++23
make
./unicagd_aml_cpp23 --count 100000
```

### 3. Modern C23 Engine
```bash
cd C23
make
./unicagd_aml_c23 --count 100000
```

### 4. Julia Safest Native `ccall` Interface
```bash
cd Julia
julia run.jl 50000
```

### 5. Node.js (TypeScript & JavaScript)
```bash
# TypeScript
cd Node.js/TypeScript
node dist/cli.js

# Zero-dependency JavaScript
cd Node.js/JavaScript
node index.js --count 50000
```

### 6. Rust FFI Crate
```bash
cd Rust
cargo run -- 50000
```

### 7. Python FFI Engine
```bash
cd Python
python3 unicagd_aml.py 50000
```

---

## 🛡️ Multi-Language Code Quality & Static Analysis Suite

Every target in the repository is validated against strict industry-standard linters and static analyzers:

| Target / Language | Linter Tool | Scope & Enforcement | Status |
| :--- | :--- | :--- | :---: |
| **C99 / C23 / C++23** | `cppcheck` + `clang-format` | Memory safety, pointer bounds, formatting, strict portability | ✅ **100% Passed (0 warnings)** |
| **Rust** | `cargo clippy` + `rustfmt` | Strict ABI memory safety, uninitialized buffer prevention, style | ✅ **100% Passed (0 warnings)** |
| **Python** | `ruff` + `mypy` | AST static analysis, unused imports, typed exception handling | ✅ **All checks passed** |
| **TypeScript / Node.js** | `tsc` + `eslint` | Strict type checking, declaration validation | ✅ **0 type errors** |
| **GPU Compute Kernels** | `glslangValidator` | Vulkan GLSL compute kernel & SPIR-V bytecode verification | ✅ **Valid SPIR-V target** |
| **Shell Scripts** | `shellcheck` | POSIX & Zsh/Bash standards, variable quoting | ✅ **0 shell warnings** |

### Run the Universal Linter Suite:
```bash
./scripts/lint_all.sh
```

```text
================================================================================
  UNICAGD_AML - MULTI-LANGUAGE CODE QUALITY & STATIC ANALYSIS SUITE              
================================================================================

[+] Linting C99, C23, C++23 with cppcheck... [PASSED] (0 warnings)
[+] Linting Rust with cargo clippy & fmt... [PASSED] (0 warnings, clean fmt)
[+] Linting Python with ruff... [PASSED] (All checks passed)
[+] Typechecking TypeScript with tsc... [PASSED] (0 type errors)
[+] Validating Vulkan GLSL Kernels... [PASSED] (Valid SPIR-V target)
[+] Checking Shell Scripts with shellcheck... [PASSED] (0 shell warnings)

✔ ALL LINTERS AND STATIC ANALYSIS CHECKS PASSED PERFECTLY!
```

---

## 📊 Terminal User Interface (TUI) & Live Viewport

The TUI features:
- **Pinned Top Dashboard:** Live stopwatch (`⏱ 00:00:00.0`), real-time throughput (`tx/s`), percentage, and alert volume that **never obstructs** the scrolling logs.
- **Pacman Progress Animation:** Polished retro character animation (`[====C...o-o]`).
- **Minimalistic Highlighting:** Normal traffic is clean and uncolored; only critical alerts and sanctions are highlighted in bold red/yellow.
- **Selectable Themes:** `--theme pacman`, `--theme cyberpunk`, `--theme matrix`, `--theme monokai`, `--theme minimal`.

---

## 📄 Multilingual HTML Master Audit Reports

Interactive, styled HTML audit reports are exported in multiple languages:
- 🇭🇺 **Hungarian Report:** [`UNICAGD_MASTER_GPU_TRANSACTION_AUDIT_REPORT_HU.html`](UNICAGD_MASTER_GPU_TRANSACTION_AUDIT_REPORT_HU.html)
- 🇬🇧 **English Report:** [`UNICAGD_MASTER_GPU_TRANSACTION_AUDIT_REPORT_EN.html`](UNICAGD_MASTER_GPU_TRANSACTION_AUDIT_REPORT_EN.html)

---

## 📦 Official Multi-Platform Releases (v1.3.0)

Precompiled native release packages for all supported platforms and architectures:

| Platform / Architecture | Release Archive | Size | Description |
| :--- | :--- | :---: | :--- |
| **macOS Apple Silicon (`arm64`)** | [`unicagd-aml-v1.3.0-macos-arm64.tar.gz`](dist/releases/unicagd-aml-v1.3.0-macos-arm64.tar.gz) | 41.5 KB | Native Apple Silicon M1-M4 (Metal GPU + SIMD) |
| **macOS Intel (`x86_64`)** | [`unicagd-aml-v1.3.0-macos-x86_64.tar.gz`](dist/releases/unicagd-aml-v1.3.0-macos-x86_64.tar.gz) | 35.2 KB | Native Intel x86_64 Mac |
| **Linux (`x86_64`)** | [`unicagd-aml-v1.3.0-linux-x86_64.tar.gz`](dist/releases/unicagd-aml-v1.3.0-linux-x86_64.tar.gz) | 74.2 KB | Native Linux x86_64 (Vulkan & CUDA) |
| **Linux ARM64 (`aarch64`)** | [`unicagd-aml-v1.3.0-linux-arm64.tar.gz`](dist/releases/unicagd-aml-v1.3.0-linux-arm64.tar.gz) | 47.6 KB | Native Linux ARM64 (aarch64) |
| **Windows (`x86_64`)** | [`unicagd-aml-v1.3.0-windows-x86_64.zip`](dist/releases/unicagd-aml-v1.3.0-windows-x86_64.zip) | 247.4 KB | Windows x86_64 (`.exe`, `.dll`, batch launcher) |
| **Windows ARM64 (`aarch64`)** | [`unicagd-aml-v1.3.0-windows-arm64.zip`](dist/releases/unicagd-aml-v1.3.0-windows-arm64.zip) | 247.4 KB | Windows on ARM64 Snapdragon / VM |
| **FreeBSD (`x86_64`)** | [`unicagd-aml-v1.3.0-freebsd-x86_64.tar.gz`](dist/releases/unicagd-aml-v1.3.0-freebsd-x86_64.tar.gz) | 32.4 KB | FreeBSD 13/14 x86_64 ELF library |
| **FreeBSD ARM64 (`aarch64`)** | [`unicagd-aml-v1.3.0-freebsd-arm64.tar.gz`](dist/releases/unicagd-aml-v1.3.0-freebsd-arm64.tar.gz) | 47.7 KB | FreeBSD 13/14 ARM64 ELF library |
| **Universal Full Source** | [`unicagd-aml-v1.3.0-universal-src.tar.gz`](dist/releases/unicagd-aml-v1.3.0-universal-src.tar.gz) | 4.06 MB | Complete source with all language bindings |

*All release archives and checksums are verified in [`dist/releases/CHECKSUMS.sha256`](dist/releases/CHECKSUMS.sha256) and [`dist/releases/RELEASE_MANIFEST_v1.3.0.json`](dist/releases/RELEASE_MANIFEST_v1.3.0.json).*

---

## ⚖️ Governance & Invariants

1. `EVIDENCE BEFORE CLAIM`: Every generated alert references concrete transaction IDs and amounts.
2. `ALERT != CRIME`: An analytical flag is an evidentiary lead, not a judicial verdict.
3. `MODEL OUTPUT != FACT`: All calculations are deterministic and mathematical without LLM hallucinations.
4. `COMPILE != VERIFIED PORT`: Full test coverage and cross-architecture verification across CPU and GPU pipelines.

---

## 📜 License
Apache License 2.0. Developed by the UNICAGD Core Team.
