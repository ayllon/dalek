use core::fmt;

static mut log_writer: Option<&'static mut fmt::Write> = None;

pub unsafe fn _print(args: fmt::Arguments) {
    use core::fmt::Write;

    if let Some(ref mut w) = log_writer {
        w.write_fmt(args);
    }
}

pub unsafe fn set_writer(w: &'static mut fmt::Write) {
    log_writer = Some(w);
}

macro_rules! print {
    ($($arg:tt)*) => ({
        unsafe { $crate::log::_print(format_args!($($arg)*)); };
    });
}

macro_rules! println {
    ($fmt:expr) => (print!(concat!($fmt, "\n")););
    ($fmt:expr, $($arg:tt)*) => (print!(concat!($fmt, "\n"), $($arg)*););
}
