use core::fmt;
use spin::{Mutex, MutexGuard};

static log_writer: Mutex<Option<*mut fmt::Write>> = Mutex::new(None);

pub fn _print(args: fmt::Arguments) -> fmt::Result {
    use core::fmt::Write;
    match *log_writer.lock() {
        Some(*mut w) => w.write_fmt(args),
        _ => Err(fmt::Error)
    }
}

macro_rules! print {
    ($($arg:tt)*) => ({
        $crate::log::_print(format_args!($($arg)*));
    });
}

macro_rules! println {
    ($fmt:expr) => (print!(concat!($fmt, "\n")));
    ($fmt:expr, $($arg:tt)*) => (print!(concat!($fmt, "\n"), $($arg)*));
}
