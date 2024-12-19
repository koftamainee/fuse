use chrono::NaiveDate;
use regex::Regex;
use std::collections::HashMap;
use std::env;
use std::fs;
use std::path::Path;
use std::process::Command;

struct Config {
    start_date: Option<NaiveDate>,
    end_date: Option<NaiveDate>,
    log_files: Vec<String>,
}

fn usage() {
    println!("Usage: fuse-analyzer [-s start_date] [-e end_date] [--files log_files]");
    println!("  -s start_date     : Specify the start date (YYYY-MM-DD)");
    println!("  -e end_date       : Specify the end date (YYYY-MM-DD)");
    println!("  --files log_files : Process specific log files (YYYY-MM-DD_hh:mm:ss.log)");
    std::process::exit(1);
}

fn parse_date(date_str: &str) -> Option<NaiveDate> {
    NaiveDate::parse_from_str(date_str, "%Y-%m-%d").ok()
}

fn get_log_files(
    log_dir: &str,
    start_date: Option<NaiveDate>,
    end_date: Option<NaiveDate>,
) -> Vec<String> {
    let mut log_files = vec![];

    if let Ok(entries) = fs::read_dir(log_dir) {
        for entry in entries {
            if let Ok(entry) = entry {
                let path = entry.path();
                if let Some(filename) = path.file_name().and_then(|f| f.to_str()) {
                    if filename.ends_with(".log") {
                        if let Some(date_str) = filename.split('_').next() {
                            if let Ok(date) = NaiveDate::parse_from_str(date_str, "%Y-%m-%d") {
                                if start_date.map_or(true, |s| date >= s)
                                    && end_date.map_or(true, |e| date <= e)
                                {
                                    log_files.push(path.to_string_lossy().to_string());
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    log_files
}

fn execute_awk_script(script_path: &str, log_path: &str) -> String {
    let output = Command::new("awk")
        .arg("-f")
        .arg(script_path)
        .arg(log_path)
        .output()
        .expect("Failed to execute awk script");

    String::from_utf8_lossy(&output.stdout).to_string()
}

fn main() {
    let log_dir = "/var/log/fuse";
    let awk_dir = "/etc/fuse/fuse-analyzer/awk";

    let args: Vec<String> = env::args().collect();
    let mut config = Config {
        start_date: None,
        end_date: None,
        log_files: vec![],
    };

    let mut i = 1;
    while i < args.len() {
        match args[i].as_str() {
            "-s" => {
                i += 1;
                if i < args.len() {
                    config.start_date = parse_date(&args[i]);
                }
            }
            "-e" => {
                i += 1;
                if i < args.len() {
                    config.end_date = parse_date(&args[i]);
                }
            }
            "--files" => {
                i += 1;
                while i < args.len() {
                    config.log_files.push(args[i].clone());
                    i += 1;
                }
            }
            _ => usage(),
        }
        i += 1;
    }

    if config.log_files.is_empty() {
        config.log_files = get_log_files(log_dir, config.start_date, config.end_date);
    }

    if config.log_files.is_empty() {
        println!("No logs found for the specified criteria.");
        return;
    }

    let mut total_execution_time = 0.0;
    let mut total_error_count = 0;
    let mut total_warn_count = 0;
    let mut total_fatal_count = 0;
    let mut total_processed_files_count = 0;
    let mut warn_lines = String::new();
    let mut error_lines = String::new();
    let mut fatal_lines = String::new();
    let mut weekdays_table = HashMap::new();
    let mut hours_table = HashMap::new();

    for log in &config.log_files {
        if let Some(filename) = Path::new(log).file_name().and_then(|f| f.to_str()) {
            if let Some(date_str) = filename.split('_').next() {
                if let Ok(date) = NaiveDate::parse_from_str(date_str, "%Y-%m-%d") {
                    let weekday = date.format("%A").to_string();
                    *weekdays_table.entry(weekday).or_insert(0) += 1;
                }

                if let Some(hour_str) = filename.split('_').nth(1).and_then(|s| s.split(':').next())
                {
                    *hours_table.entry(format!("{}:00", hour_str)).or_insert(0) += 1;
                }
            }

            let exec_time =
                execute_awk_script(&format!("{}/count_execution_time.awk", awk_dir), log);
            if let Ok(time) = exec_time.trim().parse::<f64>() {
                total_execution_time += time;
            }

            let processed_files =
                execute_awk_script(&format!("{}/count_processed_files.awk", awk_dir), log);
            let files_count = processed_files.parse::<i32>().ok();
            if let Some(count) = files_count {
                total_processed_files_count += count;
            }

            let error_output = execute_awk_script(&format!("{}/count_errors.awk", awk_dir), log);
            let re_warn = Regex::new(r"^\d{2}:\d{2}:\d{2} (WARN) .*$").unwrap();
            let re_error = Regex::new(r"^\d{2}:\d{2}:\d{2} (ERROR) .*$").unwrap();
            let re_fatal = Regex::new(r"^\d{2}:\d{2}:\d{2} (FATAL) .*$").unwrap();
            for line in error_output.lines() {
                if re_warn.is_match(line) {
                    warn_lines.push_str(line);
                    warn_lines.push('\n');
                }
                if re_error.is_match(line) {
                    error_lines.push_str(line);
                    error_lines.push('\n');
                }
                if re_fatal.is_match(line) {
                    fatal_lines.push_str(line);
                    fatal_lines.push('\n');
                }

                if line.contains("Total Warning count:") {
                    if let Some(count) = line
                        .split(':')
                        .nth(1)
                        .and_then(|s| s.trim().parse::<i32>().ok())
                    {
                        total_warn_count += count;
                    }
                } else if line.contains("Total Errors count:") {
                    if let Some(count) = line
                        .split(':')
                        .nth(1)
                        .and_then(|s| s.trim().parse::<i32>().ok())
                    {
                        total_error_count += count;
                    }
                } else if line.contains("Total Fatal errors count:") {
                    if let Some(count) = line
                        .split(':')
                        .nth(1)
                        .and_then(|s| s.trim().parse::<i32>().ok())
                    {
                        total_fatal_count += count;
                    }
                }
            }
        }
    }

    println!("[==>] Program execution times: {:}", config.log_files.len());
    let average_execution_time = total_execution_time / config.log_files.len() as f64;
    println!(
        "[==>] Average execution time: {:.4} seconds",
        average_execution_time
    );

    println!(
        "[==>] Total files processed: {}",
        total_processed_files_count
    );

    println!("[==>] Total Warnings: {}", total_warn_count);
    if total_warn_count > 0 {
        println!("---------------------------------------------------------------");
        print!("{}", warn_lines);
        println!("---------------------------------------------------------------");
    }
    println!("[==>] Total Errors: {}", total_error_count);
    if total_error_count > 0 {
        println!("---------------------------------------------------------------");
        print!("{}", error_lines);
        println!("---------------------------------------------------------------");
    }
    println!("[==>] Total Fatal Errors: {}", total_fatal_count);
    if total_fatal_count > 0 {
        println!("---------------------------------------------------------------");
        print!("{}", fatal_lines);
        println!("---------------------------------------------------------------");
    }

    println!("[==>] Weekday statistics:");
    println!("\t{:<10} {:<5}", "Day", "Count");
    for (day, count) in &weekdays_table {
        println!("\t{:<10} {}", day, count);
    }

    println!("[==>] Hour statistics:");
    println!("\t{:<10} {:<5}", "Hour", "Count");
    for (hour, count) in &hours_table {
        println!("\t{:<10} {}", hour, count);
    }
}
