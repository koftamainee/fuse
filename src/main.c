#define _POSIX_C_SOURCE 199506L

#include <bits/time.h>
#include <stdlib.h>
#include <unistd.h>

#include "../include/errors.h"
#include "../include/logger.h"
#include "cli.h"
#include "fuse.h"
#include "menu.h"

int main(int argc, char* argv[]) {
    err_t err = 0;
    struct timespec start, end;
    double time_total;
    CLIOptions options;
    char* username[256];

    clock_gettime(CLOCK_MONOTONIC, &start);

    err = parse_cli_arguments(argc, argv, &options);
    if (err != EXIT_SUCCESS && err != INVALID_CLI_ARGUMENT) {  // that is bad
        if (options.input_file != NULL) fclose(options.input_file);
        if (options.config_file != NULL) fclose(options.config_file);
        time_total =
            (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
        return err;
    }

    if (!options.quiet_mode) {
        err = logger_start();
        if (err) {
            if (options.input_file != NULL) fclose(options.input_file);
            if (options.config_file != NULL) fclose(options.config_file);
            time_total = (end.tv_sec - start.tv_sec) +
                         (end.tv_nsec - start.tv_nsec) / 1e9;
            fprintf(stderr, "Program ended with code %d in %.5f seconds", err,
                    time_total);

            return err;
        }
    }

    getlogin_r((char*)username, sizeof(username));
    log_set_level(LOG_TRACE);
    log_info("Fuse started by %s", username);

    // now logger works

    if (err == INVALID_CLI_ARGUMENT) {
        print_help();
        if (options.input_file != NULL) fclose(options.input_file);
        if (options.config_file != NULL) fclose(options.config_file);
        time_total =
            (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
        log_info("Program ended with code %d in %.5f seconds", EXIT_SUCCESS,
                 time_total);
        logger_stop();
        return err;
    }

    if (options.log_user_interaction) {
        log_set_user_interaction(1);
    }

    if (options.show_help) {
        print_help();
        if (options.input_file != NULL) fclose(options.input_file);
        if (options.config_file != NULL) fclose(options.config_file);
        time_total =
            (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
        log_info("Program ended with code %d in %.5f seconds", EXIT_SUCCESS,
                 time_total);
        logger_stop();
        return EXIT_SUCCESS;
    }

    if (options.show_version) {
        print_version();
        if (options.input_file != NULL) fclose(options.input_file);
        if (options.config_file != NULL) fclose(options.config_file);
        clock_gettime(CLOCK_MONOTONIC, &end);
        time_total =
            (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
        log_info("Program ended with code %d in %.5f seconds", EXIT_SUCCESS,
                 time_total);
        logger_stop();
        return EXIT_SUCCESS;
    }

    if (options.show_info) {
        print_info();
        if (options.input_file != NULL) fclose(options.input_file);
        if (options.config_file != NULL) fclose(options.config_file);
        clock_gettime(CLOCK_MONOTONIC, &end);
        time_total =
            (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
        log_info("Program ended with code %d in %.5f seconds", EXIT_SUCCESS,
                 time_total);
        logger_stop();

        return EXIT_SUCCESS;
    }

    if (options.interactive_menu) {
        err = start_interactive_menu(&options);
        if (options.input_file != NULL) fclose(options.input_file);
        if (options.config_file != NULL) fclose(options.config_file);
        clock_gettime(CLOCK_MONOTONIC, &end);
        time_total =
            (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
        log_info("Program ended with code %d in %.5f seconds", EXIT_SUCCESS,
                 time_total);
        logger_stop();
        return err;
    }

    if (argc == 1) {  // user didn't specify any args, starting interactive menu
        err = start_interactive_menu(&options);
        if (options.input_file != NULL) fclose(options.input_file);
        if (options.config_file != NULL) fclose(options.config_file);
        clock_gettime(CLOCK_MONOTONIC, &end);
        time_total =
            (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
        log_info("Program ended with code %d in %.5f seconds", EXIT_SUCCESS,
                 time_total);
        logger_stop();
        return err;
    }

    log_trace("Starting interpreter");

    err = fuse_start(&options);
    if (err) {
        if (options.input_file != NULL) fclose(options.input_file);
        if (options.config_file != NULL) fclose(options.config_file);
        clock_gettime(CLOCK_MONOTONIC, &end);
        time_total =
            (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
        log_info("Program ended with code %d in %.5f seconds", EXIT_SUCCESS,
                 time_total);
        logger_stop();
        return err;
    }

    if (options.input_file != NULL) fclose(options.input_file);
    if (options.config_file != NULL) fclose(options.config_file);
    clock_gettime(CLOCK_MONOTONIC, &end);
    time_total =
        (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
    log_info("Program ended with code %d in %.5f seconds", EXIT_SUCCESS,
             time_total);
    logger_stop();
    return EXIT_SUCCESS;
}
