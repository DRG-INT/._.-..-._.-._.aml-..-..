use std::env;
use std::path::PathBuf;

/*
 * Build script for Rust crate linking against C99 native library.
 * Build script a Rust crate-hez a C99 natív könyvtárhoz kapcsolásához.
 *
 * This script configures the linker to find the native C99 shared library.
 * Ez a script beállítja a linkert a natív C99 megosztott könyvtár megkereséséhez.
 */
fn main() {
    /* Get Cargo manifest directory / Cargo manifest könyvtár lekérdezése */
    let manifest_dir = env::var("CARGO_MANIFEST_DIR").expect("CARGO_MANIFEST_DIR not set");
    /* Resolve C99 directory path / C99 könyvtár elérési út feloldása */
    let c99_dir = PathBuf::from(&manifest_dir).join("../C99");
    let c99_canonical = c99_dir.canonicalize().unwrap_or(c99_dir);
    let c99_str = c99_canonical.to_str().expect("Valid UTF-8 path required");

    /* Add library search path / Könyvtár keresési útvonal hozzáadása */
    println!("cargo:rustc-link-search=native={}", c99_str);
    /* Link against unicagd_aml shared library / Linkelés unicagd_aml megosztott könyvtárhoz */
    println!("cargo:rustc-link-lib=dylib=unicagd_aml");

    /*
     * Runtime dynamic library search paths for macOS & Linux.
     * Futásidejű dinamikus könyvtár keresési útvonalak macOS és Linux alatt.
     */
    println!("cargo:rustc-link-arg=-Wl,-rpath,{}", c99_str);
    println!("cargo:rustc-link-arg=-Wl,-rpath,@loader_path/../../C99");
    println!("cargo:rustc-link-arg=-Wl,-rpath,@executable_path/../../C99");
    println!("cargo:rustc-link-arg=-Wl,-rpath,$ORIGIN/../../C99");

    /* Re-run build if these files change / Build újrafuttatása ha ezek a fájlok változnak */
    println!("cargo:rerun-if-changed=build.rs");
    println!("cargo:rerun-if-changed={}/libunicagd_aml.dylib", c99_str);
    println!("cargo:rerun-if-changed={}/libunicagd_aml.so", c99_str);
}
