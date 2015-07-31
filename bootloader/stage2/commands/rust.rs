extern {
	fn printf(format: &str);
}

#[no_mangle]
pub extern fn rust() -> i32 {
    unsafe {
        printf("Calling Rust code, and rust calling C!!\n");
	}
    return 0;
}
