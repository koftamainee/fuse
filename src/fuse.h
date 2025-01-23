#ifndef FUSE_H_
#define FUSE_H_

#include "../include/cstring.h"
#include "../include/errors.h"
#include "../include/hash_table.h"
#include "../include/trie.h"
#include "cli.h"
#include "config_parser.h"

typedef enum { unary, binary } operator_type;

typedef struct {
    operator_type type;
    int priority;
    err_t (*func)(int *, ...);
} operator_t;

err_t fuse_start(CLIOptions *cli_opts);

int is_operand(int c);

err_t create_ht_with_operators(hash_table *operators);

err_t create_ht_with_real_names(hash_table *names);

err_t execution_start(CLIOptions *cli_opts, execution_options *exec_opts,
                      String equartion_operator, String input_operator,
                      String output_operator);

err_t parse_instruction(String instruction, CLIOptions *cli_opts,
                        execution_options *exec_opts, hash_table *variables,
                        String equartion_operator, String input_operator,
                        String output_operator);

err_t parse_expression(String lvalue, String rvalue, CLIOptions *cli_opts,
                       execution_options *exec_opts, hash_table *variables);

err_t handle_input(String instruction, CLIOptions *cli_opts,
                   execution_options *exec_opts, hash_table *variables);
err_t handle_output(String instruction, CLIOptions *cli_opts,
                    execution_options *exec_opts, hash_table *variables);
#endif
