extern crate getopts;
use getopts::Options;
use std::env;
extern crate esr;
use esr::{Alignment,BoundingBox,Point};

fn print_usage(program: &str, opts: Options) {
    //let brief = format!("Usage: {} [options]", program);
    let brief = opts.short_usage(program);
    print!("{}", opts.usage(&brief));
}

fn main() {
    let args: Vec<String> = env::args().map(|x| x.to_string()).collect();

    let program = args[0].clone();
    let mut opts = Options::new();
    //opts.reqopt("a", "action", "set the action for esr, train/predict", "ACTION");
    opts.reqopt("a", "action", "set the action for esr, train/predict", "ACTION");
    opts.reqopt("i", "input", "the input configure file (for train) or image (for predict)", "INPUT");
    opts.reqopt("m", "model", "set the path to the model to store (for train) or to load (for predict)", "MODEL");
    //opts.optflag("h", "help", "print the usage and help");

    let matches = match opts.parse(&args[1..]) {
        Ok(m) => {m},
        Err(f) => {
            println!("{}", f.to_string());
            print_usage(&program, opts);
            return;
        },
    };

    /*
    if matches.opt_present("h") {
        print_usage(&program, opts);
        return;
    }
    */
    let mode = matches.opt_str("a").unwrap();
    let model_path = matches.opt_str("m").unwrap();
    let input = matches.opt_str("i").unwrap();


    println!("Welcome to our Explicit Shape Regression Implmentation");

    println!("input path: {}", input);
    println!("model path: {}", model_path);
    match mode.as_ref() {
        "train" => {
            println!("You choose to train");
            let mut alignment: Alignment = esr::Alignment::new();
            alignment.feed_sample(&input,BoundingBox::new(0f64,0f64,0f64,0f64),Vec::new());
        },
        "predict" => {
            println!("You choose to train");
        },
        _ => {
            println!("You input a wrong action, should be in {{train, predict}}");
            print_usage(&program, opts);
        },
    }
}
