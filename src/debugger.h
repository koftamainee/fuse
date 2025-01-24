#ifndef DEBUGGER_H_
#define DEBUGGER_H_

#include "../include/errors.h"
#include "../include/hash_table.h"

err_t start_debugger(hash_table *variables);

err_t debugger_print_variable(hash_table *variables);
err_t debugger_show_all_variables(hash_table *variables);
err_t debugger_change_variable(hash_table *variables);
err_t debugger_initialize_variable(hash_table *variables);
err_t debugger_deinitialize_variable(hash_table *variables);

void debugger_show_help();

#endif
