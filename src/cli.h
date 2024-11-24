#ifndef CLI_H_
#define CLI_H_

#include <stdbool.h>
#include <stdint.h>

#include "../include/cstring.h"
#include "../include/errors.h"

// Optinos fron user obtained by CLI arguments or through interattive menu
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
    bool show_help;             // -h, --help
    bool show_info;             // -i, --info
    bool show_version;          // -v, --version
} CLIOptions;

void clear_screen();

/*
 * Read CLI arguments from *argv[] and parse them into *options
 * @param argc Argument count
 * @param argv Array of String(arguments)
 * @param options Pre-allocated struct with CLI Options
 */
err_t parse_cli_arguments(int argc, char *argv[], CLIOptions *options);

/*
 * Prints help (-h, --help flags)
 */
void print_help();

/*
 * Prints info (-i, --info flags)
 */
void print_info();

/*
 * Prints version (-v, --version --ver flags)
 */
void print_version();

/*
 * Read integer number from user witout buffer overflow and integer overflow
 * @param num Int storage for data passed by reference
 */
err_t read_int_from_user(int *num);

/*
 * Read uint8_t(unsigned char) from user witout buffer overflow and integer
 * overflow
 * @param num Int storage for data passed by reference
 */
err_t read_uint8_t_from_user(uint8_t *num);

/**
 * Read String instance from user, without buffer overflow
 * @param str String type passed by reference to allocate memory;
 * @warning string_init() or string_from MUST NOT be called, pointer to
 * previosly allocated memory will be erased!!
 */
err_t read_string_from_user(String *str);

#endif