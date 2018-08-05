extern crate glob;

use std::process::Command;
use std::env;
use std::path::Path;
use glob::glob;


fn main() {
    let out_dir = env::var("OUT_DIR").unwrap();
    let cc_flags = ["-ffreestanding", "-fno-stack-protector", "-fno-builtin", "-nostdinc"];

    let asm_files: Vec<std::path::PathBuf> = glob("src/arch/x86_64/*.s").unwrap()
        .filter_map(Result::ok).collect();

    let mut object_files = Vec::new();

    for asm in asm_files {
        let asm_path = asm.to_str().unwrap();
        let asm_name = asm.file_name().unwrap().to_str().unwrap();
        let out_file = format!{"{}/{}.o", out_dir, asm_name};

        object_files.push(out_file.clone());

        let gcc_status = Command::new("gcc")
            .args(&cc_flags)
            .args(&["-o", &out_file])
            .args(&["-c", &asm_path])
            .status().expect(&("Failed to compile ".to_owned() + asm_path));
        assert!(gcc_status.success());

        println!("cargo:rerun-if-changed={}", asm.to_str().unwrap());
    }

    let ar_status = Command::new("ar")
        .args(&["crus", "libboot.a"]).args(&object_files)
        .current_dir(&Path::new(&out_dir)).status().expect("Failed to bundle libboot.a");
    assert!(ar_status.success());

    println!("cargo:rustc-link-search=native={}", out_dir);
    println!("cargo:rustc-link-lib=static=boot");
}
