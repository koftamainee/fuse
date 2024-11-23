#ifndef CLI_H_
#define CLI_H_

#include <stdbool.h>
#include <stdint.h>

#include "../include/cstring.h"
#include "../include/errors.h"

typedef struct {
    String input_file;          // -f
    String config_file;         // -c
    uint8_t base_input;         // --base_input
    uint8_t base_output;        // --base_output
    uint8_t base_assign;        // --base_assign
    bool debug_mode;            // --debug
    bool log_user_interaction;  // -d
    bool quiet_mode;            // -s
    bool preserve_temp_files;   // -t
    bool interactive_menu;      // -m
    bool show_help;             // -h
    bool show_info;             // -i
} CLIOptions;

err_t parse_cli_arguments(int argc, char *argv[], CLIOptions *options);

void print_help();
void print_info();

#endif