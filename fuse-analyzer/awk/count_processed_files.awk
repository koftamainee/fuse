BEGIN {
  files_count = 0
}

/Fuse has successfully finished program execution/ {
  files_count++
  } 

END {
  print(files_count)
}
