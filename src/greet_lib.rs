
use std::io::{self, Write};
use std::ffi::CStr;
use std::os::raw::c_char;

#[no_mangle]
pub extern "C" fn ask_name_rust(name: *mut c_char, size: usize) {
    print!("Enter your name (Rust version): ");
    io::stdout().flush().unwrap();

    let mut input = String::new();
    match io::stdin().read_line(&mut input) {
        Ok(_) => {
            let trimmed = input.trim();
            let bytes_to_copy = std::cmp::min(trimmed.len(), size - 1);

            unsafe {
                std::ptr::copy_nonoverlapping(
                    trimmed.as_ptr(),
                    name as *mut u8,
                    bytes_to_copy
                );
                *name.add(bytes_to_copy) = 0; // Null terminator
            }

            println!("Hello from Rust, {}!", trimmed);
        }
        Err(e) => {
            eprintln!("Error reading input: {}", e);
            unsafe {
                *name = 0; // Empty string on error
            }
        }
    }
}
