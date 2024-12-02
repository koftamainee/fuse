#include "menu.h"

#include <stdint.h>
#include <stdlib.h>
#include <sys/types.h>

#include "cli.h"
#include "logger.h"

void show_menu() {
    clear_screen();
    logprintf(
        "==============================\n"
        "      Interactive Menu\n"
        "==============================\n"
        "1. Run input file\n"
        "2. Load input file\n"
        "3. Load configuration\n"
        "4. Set Base input\n"
        "5. Set base output\n"
        "6. Set base assign\n"
        "7. Switch debug/release modes\n"
        "8. Show application info\n"
        "9. Help\n"
        "0. Exit\n"
        "==============================\n"
        "==> ");
}

err_t start_interactive_menu(CLIOptions* options) {
    uint8_t input_number = 0;
    int exit_from_loop = 0, continue_from_loop = 0;
    uint8_t c = 0;
    log_info("Interactive menu started");
    options->interactive_menu = 1;

    while (1) {
        show_menu();
        read_uint8_t_from_user((uint8_t*)&c);
        switch (c) {
            case 0:
                exit_from_loop = 1;
                break;

            case 26:
                exit_from_loop = 1;
                break;

            case 1:
                break;

            case 2:
                break;

            case 3:
                break;

            case 4:
                logprintf("Input base_input: ");
                read_uint8_t_from_user(&input_number);
                if (input_number < 2 || input_number > 36) {
                    clear_screen();
                    log_warn("Invalid_numeric base input");
                    logprintf("%d is not valid numeric base. Aborting.\n",
                              input_number);
                    logprintf("Press enter to continue...");
                    getchar();
                    break;
                }
                options->base_input = input_number;
                log_trace("base_input set to %d", input_number);
                logprintf("Press enter to continue...");
                getchar();
                break;

            case 5:
                logprintf("Input base_output: ");
                read_uint8_t_from_user(&input_number);
                if (input_number < 2 || input_number > 36) {
                    clear_screen();
                    log_warn("Invalid_numeric base out");
                    logprintf("%d is not valid numeric base. Aborting.\n",
                              input_number);
                    logprintf("Press enter to continue...");
                    getchar();
                    break;
                }
                options->base_output = input_number;
                log_trace("base_output set to %d", input_number);
                logprintf("Press enter to continue...");
                getchar();
                break;

            case 6:
                logprintf("Input base_assign: ");
                read_uint8_t_from_user(&input_number);
                if (input_number < 2 || input_number > 36) {
                    clear_screen();
                    log_warn("Invalid_numeric base input");
                    logprintf("%d is not valid numeric base. Aborting.\n",
                              input_number);
                    logprintf("Press enter to continue...");
                    getchar();
                    break;
                }
                options->base_assign = input_number;
                log_trace("base_assign set to %d", input_number);
                logprintf("Press enter to continue...");
                getchar();
                break;
            case 7:
                clear_screen();
                if (options->debug_mode) {
                    options->debug_mode = 0;
                    logprintf("Debug mode switched off\n");
                } else {
                    options->debug_mode = 1;
                    logprintf("Debug mode switched on\n");
                }
                logprintf("Press enter to continue...");
                getchar();
                break;

            case 8:
                clear_screen();
                print_info();
                logprintf("Press enter to continue...");
                getchar();
                break;

            case 9:
                clear_screen();
                print_help();
                logprintf("Press enter to continue...");
                getchar();
                break;
        }
        printf("\n");
        if (exit_from_loop) {
            exit_from_loop = 0;
            break;
        }
        if (continue_from_loop) {
            continue_from_loop = 0;
            continue;
        }
        clear_screen();
    }
    logprintf("Exiting gracefully.\n");
    return EXIT_SUCCESS;
}
