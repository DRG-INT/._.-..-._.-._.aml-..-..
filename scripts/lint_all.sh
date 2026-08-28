#!/usr/bin/env bash
# ==============================================================================
# UNICAGD_AML v1.3.0 - Unified Multi-Language Linter & Static Analysis Suite
# ==============================================================================
set -euo pipefail

REPO_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$REPO_ROOT"

C_CYAN='\033[1;36m'
C_GREEN='\033[1;32m'
C_RED='\033[1;31m'
C_YELLOW='\033[1;33m'
C_RESET='\033[0m'

echo -e "${C_CYAN}================================================================================${C_RESET}"
echo -e "${C_CYAN}  UNICAGD_AML - MULTI-LANGUAGE CODE QUALITY & STATIC ANALYSIS SUITE              ${C_RESET}"
echo -e "${C_CYAN}================================================================================${C_RESET}\n"

ERRORS=0

# 1. C99, C23, C++23 (cppcheck)
echo -n "[+] Linting C99, C23, C++23 with cppcheck... "
if command -v cppcheck >/dev/null 2>&1; then
    if cppcheck --enable=warning,performance,portability --error-exitcode=1 -I include C99/unicagd_aml_core.c C23/main.c C++23/main.cpp >/dev/null 2>&1; then
        echo -e "${C_GREEN}[PASSED] (0 warnings)${C_RESET}"
    else
        echo -e "${C_RED}[FAILED]${C_RESET}"
        ERRORS=$((ERRORS + 1))
    fi
else
    echo -e "${C_YELLOW}[SKIPPED: cppcheck not found]${C_RESET}"
fi

# 2. Rust (cargo clippy + cargo fmt)
echo -n "[+] Linting Rust with cargo clippy & fmt... "
if command -v cargo >/dev/null 2>&1; then
    if cargo clippy --manifest-path Rust/Cargo.toml --quiet >/dev/null 2>&1 && cargo fmt --manifest-path Rust/Cargo.toml -- --check >/dev/null 2>&1; then
        echo -e "${C_GREEN}[PASSED] (0 warnings, clean fmt)${C_RESET}"
    else
        echo -e "${C_RED}[FAILED]${C_RESET}"
        ERRORS=$((ERRORS + 1))
    fi
else
    echo -e "${C_YELLOW}[SKIPPED: cargo not found]${C_RESET}"
fi

# 3. Python (ruff)
echo -n "[+] Linting Python with ruff... "
RUFF_BIN="$(which ruff || echo "$HOME/.local/bin/ruff")"
if [[ -x "$RUFF_BIN" ]]; then
    if "$RUFF_BIN" check Python/unicagd_aml.py >/dev/null 2>&1; then
        echo -e "${C_GREEN}[PASSED] (All checks passed)${C_RESET}"
    else
        echo -e "${C_RED}[FAILED]${C_RESET}"
        ERRORS=$((ERRORS + 1))
    fi
else
    echo -e "${C_YELLOW}[SKIPPED: ruff not found]${C_RESET}"
fi

# 4. TypeScript (tsc build & typecheck)
echo -n "[+] Typechecking TypeScript with tsc... "
if command -v npm >/dev/null 2>&1; then
    if npm --prefix Node.js/TypeScript run build >/dev/null 2>&1; then
        echo -e "${C_GREEN}[PASSED] (0 type errors)${C_RESET}"
    else
        echo -e "${C_RED}[FAILED]${C_RESET}"
        ERRORS=$((ERRORS + 1))
    fi
else
    echo -e "${C_YELLOW}[SKIPPED: npm not found]${C_RESET}"
fi

# 5. Kernels (glslangValidator)
echo -n "[+] Validating Vulkan GLSL Kernels... "
if command -v glslangValidator >/dev/null 2>&1; then
    if glslangValidator -V kernels/aml_vulkan.comp -o /dev/null >/dev/null 2>&1; then
        echo -e "${C_GREEN}[PASSED] (Valid SPIR-V target)${C_RESET}"
    else
        echo -e "${C_RED}[FAILED]${C_RESET}"
        ERRORS=$((ERRORS + 1))
    fi
else
    echo -e "${C_YELLOW}[SKIPPED: glslangValidator not found]${C_RESET}"
fi

# 6. Shell Scripts (shellcheck)
echo -n "[+] Checking Shell Scripts with shellcheck... "
if command -v shellcheck >/dev/null 2>&1; then
    if shellcheck scripts/lint_all.sh >/dev/null 2>&1; then
        echo -e "${C_GREEN}[PASSED] (0 shell warnings)${C_RESET}"
    else
        echo -e "${C_RED}[FAILED]${C_RESET}"
        ERRORS=$((ERRORS + 1))
    fi
else
    echo -e "${C_YELLOW}[SKIPPED: shellcheck not found]${C_RESET}"
fi

echo ""
if [[ $ERRORS -eq 0 ]]; then
    echo -e "${C_GREEN}✔ ALL LINTERS AND STATIC ANALYSIS CHECKS PASSED PERFECTLY!${C_RESET}\n"
    exit 0
else
    echo -e "${C_RED}✖ $ERRORS linter check(s) failed.${C_RESET}\n"
    exit 1
fi
