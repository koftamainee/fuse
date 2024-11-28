#ifndef MENU_H_
#define MENU_H_

#include "../include/errors.h"
#include "cli.h"

/**
 *  Entry point to interactive menu
 * @param options Options parsed from CLI arguments
 */
err_t start_interactive_menu(CLIOptions *options);

void show_menu();

#endif