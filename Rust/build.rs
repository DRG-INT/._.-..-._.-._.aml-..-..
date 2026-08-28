fn main() {
    println!("cargo:rustc-link-search=native=/Volumes/_ARCHIVE/Developer/UNICAGD_AML/C99");
    println!("cargo:rustc-link-lib=dylib=unicagd_aml");
}
