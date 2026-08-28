use std::env;
use std::path::PathBuf;

fn main() {
    let manifest_dir = env::var("CARGO_MANIFEST_DIR").expect("CARGO_MANIFEST_DIR not set");
    let c99_dir = PathBuf::from(&manifest_dir).join("../C99");
    let c99_canonical = c99_dir.canonicalize().unwrap_or(c99_dir);
    let c99_str = c99_canonical.to_str().expect("Valid UTF-8 path required");

    println!("cargo:rustc-link-search=native={}", c99_str);
    println!("cargo:rustc-link-lib=dylib=unicagd_aml");

    // Runtime dynamic library search paths (macOS & Linux)
    println!("cargo:rustc-link-arg=-Wl,-rpath,{}", c99_str);
    println!("cargo:rustc-link-arg=-Wl,-rpath,@loader_path/../../C99");
    println!("cargo:rustc-link-arg=-Wl,-rpath,@executable_path/../../C99");
    println!("cargo:rustc-link-arg=-Wl,-rpath,$ORIGIN/../../C99");

    println!("cargo:rerun-if-changed=build.rs");
    println!("cargo:rerun-if-changed={}/libunicagd_aml.dylib", c99_str);
    println!("cargo:rerun-if-changed={}/libunicagd_aml.so", c99_str);
}
