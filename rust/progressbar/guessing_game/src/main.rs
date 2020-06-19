use std::io;
use num_cpus;
use indicatif::ProgressBar;
use std::{thread, time};

fn main() {
  let cent_millis = time::Duration::from_millis(100);
  let _now = time::Instant::now();

  println!("Guess the number!");
  println!("Please input your guess.");

  let mut guess = String::new();

  io::stdin()
    .read_line(&mut guess)
    .expect("Failed to read line");

  let saved_num_cpu = num_cpus::get() * 4;

  let bar = ProgressBar::new(saved_num_cpu as u64);
  for _ in 0..saved_num_cpu {
      bar.inc(1);
      thread::sleep(cent_millis);
    }
  bar.finish();

  println!("You guessed: {}", guess);
  println!("Hello Rust world ! {}",saved_num_cpu);
}
