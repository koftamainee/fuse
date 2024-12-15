BEGIN {
    error_count = 0
    warn_count = 0
    fatal_count = 0
    error_lines = ""
    warn_lines = ""
    fatal_lines = ""
}

/ERROR/ {
  error_count++
  error_lines = error_lines $0 "\n"
}

/WARN/ {
  warn_count++
  warn_lines = warn_lines $0 "\n"
}

/FATAL/ {
  fatal_count++
  fatal_lines = fatal_lines $0 "\n"
}

END {
  print "Total Warning count:", warn_count
  if (warn_lines != "") {
    print "Warning lines:"
    print warn_lines
  }

  print "Total Errors count:", error_count
  if (error_lines != "") {
    print "Error lines:"
    print error_lines
  }

  print "Total Fatal errors count:", fatal_count
  if (fatal_lines != "") {
    print "Fatal error lines:"
    print fatal_lines
  }
}

