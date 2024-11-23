#include "cli.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/colors.h"
#include "../include/project_version.h"

err_t parse_cli_arguments(int argc, char *argv[], CLIOptions *options) {
    int i;
    memset((void *)options, 0, sizeof(CLIOptions));

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-f") == 0 && i + 1 < argc) {
            options->input_file = argv[++i];
        } else if (strcmp(argv[i], "-c") == 0 && i + 1 < argc) {
            options->config_file = argv[++i];
        } else if (strcmp(argv[i], "-d") == 0) {
            options->log_user_interaction = true;
        } else if ((strcmp(argv[i], "-h") == 0) ||
                   strcmp(argv[i], "--help") == 0) {
            options->show_help = true;
        } else if ((strcmp(argv[i], "-i") == 0) ||
                   (strcmp(argv[i], "--info") == 0)) {
            options->show_info = true;
        } else if ((strcmp(argv[i], "-v") == 0) ||
                   (strcmp(argv[i], "--version") == 0)) {
            options->show_version = true;
        } else if (strcmp(argv[i], "-m") == 0) {
            options->interactive_menu = true;
        } else if (strcmp(argv[i], "-s") == 0) {
            options->quiet_mode = true;
        } else if (strcmp(argv[i], "-t") == 0) {
            options->preserve_temp_files = true;
        } else if (strcmp(argv[i], "--debug") == 0) {
            options->debug_mode = true;
        } else if (strcmp(argv[i], "--base_input") == 0 && i + 1 < argc) {
            options->base_input = (uint8_t)atoi(argv[++i]);
        } else if (strcmp(argv[i], "--base_output") == 0 && i + 1 < argc) {
            options->base_output = (uint8_t)atoi(argv[++i]);
        } else if (strcmp(argv[i], "--base_assign") == 0 && i + 1 < argc) {
            options->base_assign = (uint8_t)atoi(argv[++i]);
        } else {
            fprintf(stderr, COLOR_RED "%s: Unknown CLI argument.\n" COLOR_RESET,
                    argv[i]);
            return UNKNOWN_CLI_ARGUMENT;
        }
    }
    return EXIT_SUCCESS;
}

void print_help() {
    printf(COLOR_GREEN "Usage: fuse [options]\n" COLOR_RESET);
    printf(COLOR_YELLOW "Options:\n");
    printf("  -f <input_file>          Input file to process\n");
    printf("  -c <config_file>         Configuration file\n");
    printf("  -o <output_file>         Output file name\n");
    printf("  -d                       Log user interaction to .log file\n");
    printf("  -h                       Show help information\n");
    printf(
        "  -i                       Show author and university information\n");
    printf("  -m                       Start in interactive menu mode\n");
    printf("  -s                       Quiet mode (no .log file created)\n");
    printf("  -t                       Preserve temporary files\n");
    printf("  --debug                  Enable debug mode\n");
    printf("  --base_input <uint_num>  Base for input numbers\n");
    printf("  --base_output <uint_num> Base for output numbers\n");
    printf("  --base_assign <uint_num> Base for assignments\n" COLOR_RESET);
}

void print_info() {
    printf(COLOR_YELLOW "Author:" COLOR_GREEN " Friev David\n");
    printf(COLOR_YELLOW "University: " COLOR_GREEN "Kosygin University\n");
    printf(COLOR_YELLOW "Group: " COLOR_GREEN "ITPM-124\n");
    printf(
        COLOR_YELLOW
        "Professors: " COLOR_GREEN
        "A. Mokryakov\n            A. Romanenkov\n            I. Irbitsky\n");
    printf(COLOR_YELLOW "Department:" COLOR_GREEN " Computer Science\n");
}

void print_version() {
    printf(COLOR_GREEN
           "Fuse - " COLOR_RESET
           "interpreter with customizeable syntax\n" COLOR_YELLOW
           "v%s "
           "[+git]\nhttps://github.com/koftamainee/fuse.git\n" COLOR_RESET,
           PROJECT_VERSION);
}

void clear_screen() { system("clear"); }