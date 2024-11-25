#include "menu.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

#include "../include/colors.h"
#include "cli.h"
#include "logger.h"

void show_menu() {
    clear_screen();
    printf(COLOR_BRIGHT_BLUE "==============================\n" COLOR_RESET);
    printf(COLOR_BOLD_YELLOW "      Interactive Menu\n" COLOR_RESET);
    printf(COLOR_BRIGHT_BLUE "==============================\n" COLOR_RESET);

    printf(COLOR_GREEN "1. " COLOR_RESET "Load configuration\n");
    printf(COLOR_GREEN "2. " COLOR_RESET "Process input file\n");
    printf(COLOR_GREEN "3. " COLOR_RESET "Set output file\n");
    printf(COLOR_GREEN "4. " COLOR_RESET "Enable debug mode\n");
    printf(COLOR_GREEN "5. " COLOR_RESET "Show application info\n");
    printf(COLOR_GREEN "6. " COLOR_RESET "Help\n");
    printf(COLOR_RED "0. " COLOR_RESET "Exit\n");

    printf(COLOR_BRIGHT_BLUE "==============================\n" COLOR_RESET);
    printf(COLOR_CYAN "==> " COLOR_RESET);
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
    printf("Exiting gracefully.\n");
    return EXIT_SUCCESS;
}
