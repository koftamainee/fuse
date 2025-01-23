#ifndef POSTFIX_NOTATION_H_
#define POSTFIX_NOTATION_H_

#include "fuse.h"

err_t execute_postfix_expression(String lvalue, String rvalue,
                                 CLIOptions *cli_opts,
                                 execution_options *exec_opts,
                                 hash_table *variables);

err_t calculate_postfix_expression(const String postfix_exp,
                                   int *expression_result,
                                   hash_table *operators,
                                   hash_table *variables);

#endif  // !POSTFIX_NOTATION_H_
