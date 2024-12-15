/Program ended with code/ {
    match($0, /in ([0-9.]+) seconds/, result)
    if (result[1] != "") {
        exec_time = result[1]
        print exec_time
    }
}
