#ifndef INFIX_NOTATION_H_
#define INFIX_NOTATION_H_

#include "../include/cstring.h"
#include "../include/errors.h"
#include "../include/hash_table.h"
#include "../include/u_list.h"
#include "cli.h"
#include "fuse.h"

err_t execute_infix_expression(String lvalue, String rvalue,
                               CLIOptions *cli_opts,
                               execution_options *exec_opts,
                               hash_table *variables);

err_t infix_to_postfix(const String infix_exp, int (*is_operand)(int c),
                       hash_table *operators_ht, String *postfix_exp);
#endif
