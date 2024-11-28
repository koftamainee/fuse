#include "cli.h"

#include <limits.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#include "../include/project_version.h"
#include "logger.h"

err_t parse_cli_arguments(int argc, char *argv[], CLIOptions *options) {
    int i;

    if (options == NULL || argv == NULL) {
        return DEREFERENCING_NULL_PTR;
    }
    memset((void *)options, 0, sizeof(CLIOptions));

    for (i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-s") == 0) {
            options->quiet_mode = 1;
        } else if (strcmp(argv[i], "-t") == 0) {
            options->preserve_temp_files = 1;
        } else if (strcmp(argv[i], "-f") == 0 && i + 1 < argc) {
            options->input_file = string_from(argv[++i]);
        } else if (strcmp(argv[i], "-c") == 0 && i + 1 < argc) {
            options->config_file = string_from(argv[++i]);
        } else if (strcmp(argv[i], "-d") == 0) {
            options->log_user_interaction = 1;
        } else if ((strcmp(argv[i], "-h") == 0) ||
                   strcmp(argv[i], "--help") == 0) {
            options->show_help = 1;
        } else if ((strcmp(argv[i], "-i") == 0) ||
                   (strcmp(argv[i], "--info") == 0)) {
            options->show_info = 1;
        } else if ((strcmp(argv[i], "-v") == 0) ||
                   (strcmp(argv[i], "--version") == 0) ||
                   strcmp(argv[i], "--ver") == 0) {
            options->show_version = 1;
        } else if (strcmp(argv[i], "-m") == 0) {
            options->interactive_menu = 1;
        } else if (strcmp(argv[i], "-s") == 0) {
            options->quiet_mode = 1;
        } else if (strcmp(argv[i], "-t") == 0) {
            options->preserve_temp_files = 1;
        } else if (strcmp(argv[i], "--debug") == 0) {
            options->debug_mode = 1;
        } else if (strcmp(argv[i], "--base_input") == 0 && i + 1 < argc) {
            options->base_input = (uint8_t)atoi(argv[++i]);
        } else if (strcmp(argv[i], "--base_output") == 0 && i + 1 < argc) {
            options->base_output = (uint8_t)atoi(argv[++i]);
        } else if (strcmp(argv[i], "--base_assign") == 0 && i + 1 < argc) {
            options->base_assign = (uint8_t)atoi(argv[++i]);
        } else {
            fprintf(stderr, "%s: Unknown CLI argument.\n", argv[i]);
            return UNKNOWN_CLI_ARGUMENT;
        }
    }
    return EXIT_SUCCESS;
}

void print_help() {
    logprintf(
        "Usage: fuse [options]\n"
        "Options:\n"
        "  -f <input_file>          Input file to process\n"
        "  -c <config_file>         Configuration file\n"
        "  -o <output_file>         Output file name\n"
        "  -d                       Log user interaction to .log file\n"
        "  -h                       Show help information\n"
        "  -i                       Show author and university information\n"
        "  -m                       Start in interactive menu mode\n"
        "  -s                       Quiet mode (no .log file created)\n"
        "  -t                       Preserve temporary files\n"
        "  --debug                  Enable debug mode\n"
        "  --base_input <uint_num>  Base for input numbers\n"
        "  --base_output <uint_num> Base for output numbers\n"
        "  --base_assign <uint_num> Base for assignments\n");
}

void print_info() {
    logprintf(
        "Author:     Friev David\n"
        "University: Kosygin University\n"
        "Group:      ITPM-124\n"
        "Professors: A. Mokryakov\n"
        "            A. Romanenkov\n"
        "            I. Irbitsky\n"
        "Department: Computer Science\n");
}

void print_version() {
    logprintf(
        "Fuse - interpreter with customizeable syntax\n"
        "v%s "
        "[+git]\nhttps://github.com/koftamainee/fuse.git\n",
        PROJECT_VERSION);
}

void clear_screen() { system("clear"); }

err_t read_int_from_user(int *num) {
    char buf[32];  // 32 bytes buf for int32_t
    char *endptr = NULL;
    long value = 0;  // at least 32+ bytes for handling integer overflow

    if (num == NULL) {
        log_error("Passed pointer to read_int function is NULL");
        return DEREFERENCING_NULL_PTR;
    }

    if (!fgets(buf, sizeof(buf), stdin)) {
        log_error("error while reading udata from stdin");
        return ERROR_READING_FROM_STDIN;
    }

    value = strtol(buf, &endptr, 10);
    log_io("Input: %ld", value);

    if (*endptr == '\0' || *endptr == '\n') {      // if read all data
        if (value > INT_MAX || value < INT_MIN) {  // and integer overflow
            log_error("%ld integer overflow", value);
            return INTEGER_OVERFLOW;
        }
    }
    // all ok now

    *num = (int)value;

    return EXIT_SUCCESS;
}

// same as prev function but other limits
err_t read_uint8_t_from_user(uint8_t *num) {
    char buf[8];  // 32 bytes buf for int32_t
    char *endptr = NULL;
    long value = 0;  // at least 32+ bytes for handling integer overflow

    if (num == NULL) {
        log_error("Passed pointer to read_uint8_t function is NULL");
        return DEREFERENCING_NULL_PTR;
    }

    if (!fgets(buf, sizeof(buf), stdin)) {
        log_error("error while reading udata from stdin");
        return ERROR_READING_FROM_STDIN;
    }

    value = strtol(buf, &endptr, 10);
    log_io("Input: %ld", value);

    if (*endptr == '\0' || *endptr == '\n') {        // if read all data
        if (value > CHAR_MAX || value < CHAR_MIN) {  // and integer overflow
            log_error("%ld uint8_t overflow", value);
            return UINT8_T_OVERFLOW;
        }
    }
    // all ok now

    *num = (uint8_t)value;

    return EXIT_SUCCESS;
}

err_t read_string_from_user(String *str) {
    size_t len = 0;
    char buf[BUFSIZ];

    if (str == NULL) {
        log_error("Passed pointer to read_string function is NULL");
        return DEREFERENCING_NULL_PTR;
    }

    if (*str != NULL) {  // potentioal memory leak
        log_warn(
            "Passed string to read_string function is not NULL. Potencial "
            "memory leak");
    }

    if (!fgets(buf, BUFSIZ, stdin)) {
        log_error("error while reading udata from stdin");
        return ERROR_READING_FROM_STDIN;
    }
    len = strlen(buf);
    if (len > 0 && buf[len - 1] == '\n') {
        buf[len = 1] = '\0';  // delete '\n' symbol
    }

    log_io("Input: %s");

    *str = string_from(buf);  // allocating memory for string
    if (*str == NULL) {
        return MEMORY_ALLOCATION_ERROR;
    }

    return EXIT_SUCCESS;
}

void logprintf(const char *_format, ...) {
    va_list ap, ap_cpy;
    va_start(ap, _format);
    va_copy(ap_cpy, ap);
    vprintf(_format, ap_cpy);
    va_end(ap_cpy);
    va_copy(ap_cpy, ap);
    vlog_log(LOG_IO, __FILE__, __LINE__, _format,
             ap_cpy);  // using this instead of log_io() to pass va_list
                       // pointer as argument
}
