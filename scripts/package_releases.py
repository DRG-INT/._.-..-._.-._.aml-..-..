#!/usr/bin/env python3
"""
UNICAGD_AML Multi-Architecture Release Packager & Checksum Generator.
UNICAGD_AML Több-architektúrás Release Csomagoló és Ellenőrzőösszeg Generátor.

Packages all platform-specific binaries into release archives and generates checksums.
Csomagolja az összes platform-specifikus binárist release archívumba és generálja az ellenőrzőösszegeket.
"""

import os
import tarfile
import zipfile
import hashlib
import json

"""
Repository root directory.
Repository gyökér könyvtár.
"""
REPO_ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), ".."))
"""
Releases output directory.
Release kimeneti könyvtár.
"""
RELEASES_DIR = os.path.join(REPO_ROOT, "dist", "releases")
os.makedirs(RELEASES_DIR, exist_ok=True)

"""
Calculate SHA256 checksum of a file.
SHA256 ellenőrzőösszeg számítása egy fájlhoz.

@param filepath Path to file / Fájl elérési útja.
@return SHA256 hex digest / SHA256 hex digest.
"""
def sha256_file(filepath):
    h = hashlib.sha256()
    with open(filepath, 'rb') as f:
        while chunk := f.read(65536):
            h.update(chunk)
    return h.hexdigest()

"""
Add common documentation files to archive.
Közös dokumentációs fájlok hozzáadása az archívumhoz.

@param archive Archive object / Archívum objektum.
@param is_zip True for ZIP, False for TAR / True ZIP-hez, False TAR-hoz.
"""
def add_common_docs(archive, is_zip=False):
    common_files = [
        "UNICAGD_MASTER_GPU_TRANSACTION_AUDIT_REPORT_HU.html",
        "UNICAGD_MASTER_GPU_TRANSACTION_AUDIT_REPORT_EN.html",
        "README.md"
    ]
    for doc in common_files:
        full = os.path.join(REPO_ROOT, doc)
        if os.path.exists(full):
            if is_zip:
                archive.write(full, arcname=doc)
            else:
                archive.add(full, arcname=doc)

"""
Add kernel files to archive.
Kernel fájlok hozzáadása az archívumhoz.

@param archive Archive object / Archívum objektum.
@param is_zip True for ZIP, False for TAR / True ZIP-hez, False TAR-hoz.
"""
def add_kernels(archive, is_zip=False):
    kernel_dir = os.path.join(REPO_ROOT, "kernels")
    if os.path.exists(kernel_dir):
        for s in os.listdir(kernel_dir):
            if s.startswith('.'): continue
            full = os.path.join(kernel_dir, s)
            if is_zip:
                archive.write(full, arcname=f"kernels/{s}")
            else:
                archive.add(full, arcname=f"kernels/{s}")

"""
Create tar.gz release archive for Unix platforms.
tar.gz release archívum létrehozása Unix platformokhoz.

@param name Archive base name / Archívum alap név.
@param bin_dir Binary directory / Bináris könyvtár.
"""
def make_tar_release(name, bin_dir):
    tar_path = os.path.join(RELEASES_DIR, f"{name}.tar.gz")
    with tarfile.open(tar_path, "w:gz") as tar:
        if os.path.exists(bin_dir):
            for b in os.listdir(bin_dir):
                if b.startswith('.'): continue
                tar.add(os.path.join(bin_dir, b), arcname=f"bin/{b}")
        add_kernels(tar, is_zip=False)
        add_common_docs(tar, is_zip=False)
    print(f"✔ Created: {tar_path}")

"""
Create zip release archive for Windows platforms.
zip release archívum létrehozása Windows platformokhoz.

@param name Archive base name / Archívum alap név.
@param bin_dir Binary directory / Bináris könyvtár.
"""
def make_zip_release(name, bin_dir):
    zip_path = os.path.join(RELEASES_DIR, f"{name}.zip")
    with zipfile.ZipFile(zip_path, "w", zipfile.ZIP_DEFLATED) as zf:
        if os.path.exists(bin_dir):
            for b in os.listdir(bin_dir):
                if b.startswith('.'): continue
                zf.write(os.path.join(bin_dir, b), arcname=f"bin/{b}")
        bat_content = "@echo off\r\necho Starting UNICAGD_AML...\r\nbin\\unicagd_aml.exe --count 50000 --theme pacman\r\npause\r\n"
        zf.writestr("run_simulation.bat", bat_content)
        add_kernels(zf, is_zip=True)
        add_common_docs(zf, is_zip=True)
    print(f"✔ Created: {zip_path}")

"""
Create universal source archive.
Univerzális forrás archívum létrehozása.
"""
def make_universal_src():
    tar_path = os.path.join(RELEASES_DIR, "unicagd-aml-v1.3.0-universal-src.tar.gz")
    with tarfile.open(tar_path, "w:gz") as tar:
        for folder in ["C99", "C23", "C++23", "Julia", "Node.js", "Python", "Rust", "kernels", "include", "docs", "scripts"]:
            full = os.path.join(REPO_ROOT, folder)
            if os.path.exists(full):
                tar.add(full, arcname=folder)
        add_common_docs(tar, is_zip=False)
    print(f"✔ Created: {tar_path}")

"""
Main packaging orchestrator.
Fő csomagolási összehangoló.
"""
def main():
    print("================================================================================")
    print(" 📦 UNICAGD_AML v1.3.0 - FULL MULTI-ARCHITECTURE RELEASE PACKAGING")
    print("================================================================================\n")

    # 1. macOS / macOS
    make_tar_release("unicagd-aml-v1.3.0-macos-arm64", os.path.join(REPO_ROOT, "dist", "bin", "macos_arm64"))
    make_tar_release("unicagd-aml-v1.3.0-macos-x86_64", os.path.join(REPO_ROOT, "dist", "bin", "macos_x86_64"))

    # 2. Linux / Linux
    make_tar_release("unicagd-aml-v1.3.0-linux-x86_64", os.path.join(REPO_ROOT, "dist", "bin", "linux_x86_64"))
    make_tar_release("unicagd-aml-v1.3.0-linux-arm64", os.path.join(REPO_ROOT, "dist", "bin", "linux_arm64"))

    # 3. Windows / Windows
    make_zip_release("unicagd-aml-v1.3.0-windows-x86_64", os.path.join(REPO_ROOT, "dist", "bin", "windows_x86_64"))
    make_zip_release("unicagd-aml-v1.3.0-windows-arm64", os.path.join(REPO_ROOT, "dist", "bin", "windows_x86_64"))

    # 4. FreeBSD / FreeBSD
    make_tar_release("unicagd-aml-v1.3.0-freebsd-x86_64", os.path.join(REPO_ROOT, "dist", "bin", "freebsd_x86_64"))
    make_tar_release("unicagd-aml-v1.3.0-freebsd-arm64", os.path.join(REPO_ROOT, "dist", "bin", "freebsd_arm64"))

    # 5. Universal Full Source / Univerzális teljes forrás
    make_universal_src()

    """
    Generate release manifest and checksums.
    Release jegyzék és ellenőrzőösszegek generálása.
    """
    manifest = {
        "project": "UNICAGD_AML",
        "version": "1.3.0",
        "release_timestamp": "2026-08-28T16:45:00Z",
        "architectures": [
            "macos-arm64",
            "macos-x86_64",
            "linux-x86_64",
            "linux-arm64",
            "windows-x86_64",
            "windows-arm64",
            "freebsd-x86_64",
            "freebsd-arm64"
        ],
        "artifacts": []
    }

    checksum_lines = []
    for f in sorted(os.listdir(RELEASES_DIR)):
        if f.endswith(('.tar.gz', '.zip')):
            fp = os.path.join(RELEASES_DIR, f)
            csum = sha256_file(fp)
            size = os.path.getsize(fp)
            manifest["artifacts"].append({
                "filename": f,
                "size_bytes": size,
                "sha256": csum
            })
            checksum_lines.append(f"{csum}  {f}")

    with open(os.path.join(RELEASES_DIR, "RELEASE_MANIFEST_v1.3.0.json"), "w") as mf:
        json.dump(manifest, mf, indent=2)

    with open(os.path.join(RELEASES_DIR, "CHECKSUMS.sha256"), "w") as cf:
        cf.write("\n".join(checksum_lines) + "\n")

    print("\n================================================================================")
    print(" 🏁 ALL 8 PLATFORM RELEASES PACKAGED & HASHED")
    print("================================================================================\n")
    for a in manifest["artifacts"]:
        print(f"  • {a['filename']:<42} ({a['size_bytes']:>9,} B) | SHA256: {a['sha256'][:24]}...")
    print("================================================================================\n")

if __name__ == '__main__':
    main()
