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
        "1. Load configuration\n"
        "2. Process input file\n"
        "3. Set output file\n"
        "4. Enable debug mode\n"
        "5. Show application info\n"
        "6. Help\n"
        "0. Exit\n"
        "==============================\n"
        "==> ");
}

err_t start_interactive_menu(CLIOptions* options) {
    char c = 0;
    log_info("Interactive menu started");
    while (1) {
        show_menu();
        read_uint8_t_from_user((uint8_t*)&c);
        if (c == 0) {
            break;
        }
        printf("\n");
    }
    logprintf("Exiting gracefully.\n");
    return EXIT_SUCCESS;
}
