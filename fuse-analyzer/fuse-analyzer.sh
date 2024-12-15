#!/bin/bash

LOG_DIR="/var/log/fuse"
AWK_DIR="/etc/fuse/fuse-analyzer/awk"

START_DATE=""
END_DATE=""
LOG_FILES=()

usage() {
    echo "Usage: fuse-analyzer [-s start_date] [-e end_date] [-f log_file] [--files]"
    echo "  -s start_date     : Specify the start date (YYYY-MM-DD)"
    echo "  -e end_date       : Specify the end date (YYYY-MM-DD)"
    echo "  --files log_files : Process a specific log files. (YYYY-MM-DD_hh:mm:ss.log)"
    exit 1
}

check_date() {
  DATE_REGEX='^[0-9]{4}-(0[1-9]|1[012])-(0[1-9]|[12][0-9]|3[01])$'
  
  if [[ $1 =~ $DATE_REGEX ]]; then
    return 1
  else
    return 0
  fi
}

check_log_filename() {
  LOGFILE_REGEX='d{4}-\d{2}-\d{2}_\d{2}:\d{2}:\d{2}\.log$'

  if [[ $1 =~ $LOGFILE_REGEX ]]; then
    return 1
  else
    return 0
  fi
}

get_files_flag() {
  while [[ $# -gt 0 ]]; do
    if check_log_filename "$1"; then
      if [ ! -f "/var/log/fuse/$1" ]; then
        echo "ERROR: Log file $1 doesn't exists"
        shift
        continue
      fi
      LOG_FILES+=("/var/log/fuse/$1")
      shift
    else
      echo "Invalid log file name"
      usage
    fi
  done
}

while [[ $# -gt 0 ]]; do
    case "$1" in
        -s) START_DATE="$2"; shift 2
          if check_date "$START_DATE"; then
            echo "Invalid data format: $START_DATE"
            usage
          fi
          ;;
        -e) END_DATE="$2"; shift 2 
           if check_date "$END_DATE"; then
            echo "Invalid data format: $END_DATE"
            usage
          fi

          ;;
        --files) shift; get_files_flag "$@"; break ;;
        *) echo "Invalid: $1"; usage ;;
    esac
done

if [ -z "$( ls -A $LOG_DIR )" ]; then
  echo "Any log files doesnt found."
  exit 1
fi

if [ ${#LOG_FILES[@]} -eq 0 ]; then
    for log in "$LOG_DIR"/*.log; do
        LOG_DATE=$(basename "$log" | cut -d_ -f1)

        if [[ -n $START_DATE && $LOG_DATE < $START_DATE ]]; then
          continue
        fi
        if [[ -n $END_DATE && $LOG_DATE > $END_DATE ]]; then
            continue
        fi
        LOG_FILES+=("$log")
    done
fi


if [ ${#LOG_FILES[@]} -eq 0 ]; then
    echo "No logs found for the specified criteria."
    exit 1
fi

weekdays_table=""
hours_table=""

for log in "${LOG_FILES[@]}" 
do

  date=$(basename "$log" | cut -d'_' -f1) 
  day_of_week=$(date -d "$date" +"%A") 
  weekdays_table+="$day_of_week"$'\n'   

  hour=$(basename "$log" | cut -d'_' -f2 | cut -d':' -f1) 
  hours_table+="$hour:00"$'\n'

  output=$(awk -f "$AWK_DIR"/count_execution_time.awk "$log")
  total_execution_time=$(awk -v a="$total_execution_time" -v b="$output" 'BEGIN { print a + b }')

  output=$(awk -f "$AWK_DIR"/count_errors.awk "$log")

  error_count=$(echo "$output" | grep -oP "Total Errors count: \K\d+")
  warn_count=$(echo "$output" | grep -oP "Total Warning count: \K\d+")
  fatal_count=$(echo "$output" | grep -oP "Total Fatal errors count: \K\d+")

  error_lines=$(echo "$output" | sed -n '/Error lines:/,/^$/p' | sed '1d')
  warn_lines=$(echo "$output" | sed -n '/Warning lines:/,/^$/p' | sed '1d')
  fatal_lines=$(echo "$output" | sed -n '/Fatal error lines:/,/^$/p' | sed '1d')

  total_error_count=$((total_error_count + error_count))
  total_warn_count=$((total_warn_count + warn_count))
  total_fatal_count=$((total_fatal_count + fatal_count))

  if [ -n "$error_lines" ]
  then
    error_lines="$error_lines\n"
  fi

  if [ -n "$warn_lines" ]
  then
    warn_lines="$warn_lines\n"
  fi

  if [ -n "$fatal_lines" ]
  then
    fatal_lines="$fatal_lines\n"
  fi
  
  total_error_lines="$total_error_lines$error_lines"
  total_warn_lines="$total_warn_lines$warn_lines"
  total_fatal_lines="$total_fatal_lines$fatal_lines"

  output=$(awk -f "$AWK_DIR"/count_processed_files.awk "$log")
  total_files_count=$((total_files_count + output))

done

weekdays_table=$(echo "$weekdays_table" | head -c -1 | sort | uniq -c | sort -nr)
hours_table=$(echo "$hours_table" | head -c -1 | sort | uniq -c | sort -nr)

# finalle
echo -e "[==>]  Program execution times: ${#LOG_FILES[@]}"
execution_count=${#LOG_FILES[@]}
average_execution_time=$(awk -v total="$total_execution_time" -v count="$execution_count" 'BEGIN { print total / count }')
echo -e "[==>]  Average execution time: $average_execution_time seconds"

echo -e "[==>]  Total file processed: $total_files_count"

echo "[==>]  Total Warning count: $total_warn_count"
if [ -n "$total_warn_lines" ]; then
  total_warn_lines=$(echo "$total_warn_lines" | head -c -3)
  echo -e "---------------------------------------------------------------"
  echo -e "$total_warn_lines"
  echo -e "---------------------------------------------------------------"
fi

echo -e "[==>]  Total Errors count: $total_error_count"
if [ -n "$total_error_lines" ]; then
  total_error_lines=$(echo "$total_error_lines" | head -c -3)
  echo -e "---------------------------------------------------------------"
  echo -e "$total_error_lines"
  echo -e "---------------------------------------------------------------"
fi

echo -e "[==>]  Total Fatal errors count: $total_fatal_count"
if [ -n "$total_fatal_lines" ]; then
  total_fatal_lines=$(echo "$total_fatal_lines" | head -c -3)
  echo -e "---------------------------------------------------------------"
  echo -e "$total_fatal_lines"
  echo -e "---------------------------------------------------------------"
fi


echo -e "[==>]  Week-days statistics:\n$weekdays_table"

echo -e "[==>]  Hours statistics:\n$hours_table"


