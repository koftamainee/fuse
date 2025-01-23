#ifndef CONFIG_PARSER_H_
#define CONFIG_PARSER_H_

#include "../include/cstring.h"
#include "../include/errors.h"
#include "../include/hash_table.h"

typedef enum { left_eq, right_eq } equation_type;
typedef enum { prefix, postfix, infix } expression_type;

typedef struct execution_options {
    equation_type eq_t;
    expression_type ex_t;
    hash_table *operators;
} execution_options;

err_t parse_config_file(FILE *config_file, execution_options *options,
                        String *equation_operator_placeholder,
                        String *input_operator_placeholder,
                        String *output_operator_placeholder);

err_t set_operator(hash_table *operators, String old_rep, String new_rep);

#endif
