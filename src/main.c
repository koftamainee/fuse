#define _POSIX_C_SOURCE 199309L

#include <bits/time.h>
#include <stdio.h>
#include <stdlib.h>

#include "../include/errors.h"
#include "cli.h"
#include "logger.h"
#include "menu.h"

int main(int argc, char* argv[]) {
  err_t err = 0;
  struct timespec start, end;
  double time_total;
  CLIOptions options;

  clock_gettime(CLOCK_MONOTONIC, &start);

  /* TEMPORARY: creating logger instances */
  FILE* fptr = fopen("fuse.log", "w");
  log_set_level(LOG_TRACE);
  log_add_fp(fptr, LOG_TRACE);
  log_add_fp(stderr, LOG_ERROR);

  log_info("Fuse started");

  err = parse_cli_arguments(argc, argv, &options);
  if (err != 0 && err != UNKNOWN_CLI_ARGUMENT) {  // that is bad
    string_free(options.input_file);
    string_free(options.config_file);
    log_info("Program ended with code %d", err);
    return err;
  }

  if (options.log_user_interaction) {
    log_set_user_interaction(1);
  }

  if (options.show_help || err == UNKNOWN_CLI_ARGUMENT) {
    print_help();
    string_free(options.input_file);
    string_free(options.config_file);
    log_info("Program ended with code %d", err);
    return EXIT_SUCCESS;
  }

  if (options.show_version) {
    print_version();
    string_free(options.input_file);
    string_free(options.config_file);
    log_info("Program ended with code %d", err);
    return EXIT_SUCCESS;
  }

  if (options.show_info) {
    print_info();
    string_free(options.input_file);
    string_free(options.config_file);
    log_info("Program ended with code %d", err);
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
  clock_gettime(CLOCK_MONOTONIC, &end);
  time_total =
      (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
  log_info("Program ended with code %d in %.5f seconds", EXIT_SUCCESS,
           time_total);
  return EXIT_SUCCESS;
}
