#include <stdlib.h>

#include "../include/errors.h"
#include "cli.h"
#include "menu.h"

int main(int argc, char* argv[]) {
    err_t err = 0;
    CLIOptions options;

    err = parse_cli_arguments(argc, argv, &options);
    if (err != 0 && err != UNKNOWN_CLI_ARGUMENT) {  // that is bad
        // TODO
        string_free(options.input_file);
        string_free(options.config_file);
        return err;
    }

    if (options.show_help || err == UNKNOWN_CLI_ARGUMENT) {
        print_help();
        string_free(options.input_file);
        string_free(options.config_file);
        return EXIT_SUCCESS;
    }

    if (options.show_version) {
        print_version();
        string_free(options.input_file);
        string_free(options.config_file);
        return EXIT_SUCCESS;
    }

    if (options.show_info) {
        print_info();
        string_free(options.input_file);
        string_free(options.config_file);
        return EXIT_SUCCESS;
    }

    if (options.interactive_menu) {
        err = start_interactive_menu(&options);
        if (err) {
            // TODO
        }
    }

    if (argc == 1) {  // user didn't specify any args, starting interactive menu
        err = start_interactive_menu(&options);
        if (err) {
            // TODO
        }
    }

    string_free(options.input_file);
    string_free(options.config_file);
    return EXIT_SUCCESS;
}
