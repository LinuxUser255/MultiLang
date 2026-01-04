use std::io;

// What is io::Result<()> in main?
// Allows main to return errors from I/O ops (e.g., file reads), exiting with non-zero code on failure.
// Success: Ok(()) (no value, just done).
// Failure: Err(e) — Rust prints error and exits non-zero.
// Great for clean error propagation in servers/scripts without manual handling.
pub fn main() -> io::Result<()> {
    println!("Enter your name: ");

    let mut input = String::new();
    match io::stdin().read_line(&mut input) {
        Ok(_) => {
            println!("Hello {} from Rust!", input.trim());
            Ok(())
        }
        Err(e) => {
            eprintln!("Error reading input: {}", e);
            Err(e)
        }
    }
}

