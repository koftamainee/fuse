#include "fuse.h"

#include <assert.h>
#include <ctype.h>
#include <limits.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/logger.h"
#include "../include/types.h"
#include "config_parser.h"
#include "infix_notation.h"
#include "postfix_notation.h"

int isspace_c(int c) {
    int ret = isspace(c);
    if (ret) {
        return ret;
    }
    if (c == '\n' || c == '\0' || c == -1 || c == 8) {
        return 1;
    }
    return 0;
}

err_t fuse_not(int *res, ...) {
    if (res == NULL) {
        log_fatal("passed ptr is NULL");
        return DEREFERENCING_NULL_PTR;
    }

    va_list ap;
    va_start(ap, res);
    *res = ~va_arg(ap, int);
    va_end(ap);

    return EXIT_SUCCESS;
}

err_t fuse_add(int *res, ...) {
    if (res == NULL) {
        log_fatal("passed ptr is NULL");
        return DEREFERENCING_NULL_PTR;
    }

    va_list ap;
    va_start(ap, res);
    *res = va_arg(ap, int) + va_arg(ap, int);
    va_end(ap);

    return EXIT_SUCCESS;
}

err_t fuse_mult(int *res, ...) {
    if (res == NULL) {
        log_fatal("passed ptr is NULL");
        return DEREFERENCING_NULL_PTR;
    }

    va_list ap;
    va_start(ap, res);
    *res = va_arg(ap, int) * va_arg(ap, int);
    va_end(ap);

    return EXIT_SUCCESS;
}

err_t fuse_sub(int *res, ...) {
    if (res == NULL) {
        log_fatal("passed ptr is NULL");
        return DEREFERENCING_NULL_PTR;
    }

    va_list ap;
    va_start(ap, res);
    *res = va_arg(ap, int) - va_arg(ap, int);
    va_end(ap);

    return EXIT_SUCCESS;
}

err_t fuse_pow(int *res, ...) {
    if (res == NULL) {
        log_fatal("passed ptr is NULL");
        return DEREFERENCING_NULL_PTR;
    }

    va_list ap;
    va_start(ap, res);
    int base = va_arg(ap, int);
    int exp = va_arg(ap, int);
    int mod = va_arg(ap, int);
    va_end(ap);

    int result = 1;
    base %= mod;
    while (exp > 0) {
        if (exp % 2 == 1) {
            result = (result * base) % mod;
        }
        exp /= 2;
        base = (base * base) % mod;
    }

    *res = result;
    return EXIT_SUCCESS;
}

err_t fuse_div(int *res, ...) {
    if (res == NULL) {
        log_fatal("passed ptr is NULL");
        return DEREFERENCING_NULL_PTR;
    }

    va_list ap;
    va_start(ap, res);
    int a = va_arg(ap, int);
    int b = va_arg(ap, int);
    va_end(ap);

    if (b == 0) {
        log_fatal("division by zero");
        return ZERO_DIVISION;
    }

    *res = a / b;
    return EXIT_SUCCESS;
}

err_t fuse_rem(int *res, ...) {
    if (res == NULL) {
        log_fatal("passed ptr is NULL");
        return DEREFERENCING_NULL_PTR;
    }

    va_list ap;
    va_start(ap, res);
    int a = va_arg(ap, int);
    int b = va_arg(ap, int);
    va_end(ap);

    if (b == 0) {
        log_fatal("division by zero");
        return ZERO_DIVISION;
    }

    *res = a % b;
    return EXIT_SUCCESS;
}

err_t fuse_xor(int *res, ...) {
    if (res == NULL) {
        log_fatal("passed ptr is NULL");
        return DEREFERENCING_NULL_PTR;
    }

    va_list ap;
    va_start(ap, res);
    *res = va_arg(ap, int) ^ va_arg(ap, int);
    va_end(ap);

    return EXIT_SUCCESS;
}

err_t fuse_and(int *res, ...) {
    if (res == NULL) {
        log_fatal("passed ptr is NULL");
        return DEREFERENCING_NULL_PTR;
    }

    va_list ap;
    va_start(ap, res);
    *res = va_arg(ap, int) & va_arg(ap, int);
    va_end(ap);

    return EXIT_SUCCESS;
}

err_t fuse_or(int *res, ...) {
    if (res == NULL) {
        log_fatal("passed ptr is NULL");
        return DEREFERENCING_NULL_PTR;
    }

    va_list ap;
    va_start(ap, res);
    *res = va_arg(ap, int) | va_arg(ap, int);
    va_end(ap);

    return EXIT_SUCCESS;
}

int operators_string_comparer(const void *a, const void *b) {
    return string_cmp(*(const String *)a, *(const String *)b);
}
void operators_bucket_destructor(void *a) {
    hash_table_bucket *b = a;
    string_free(*(String *)b->key);
    free(b->key);
    free(b->value);
    free(b);
}

err_t fuse_start(CLIOptions *cli_opts) {
    if (cli_opts == NULL) {
        log_fatal("passed ptr is NULL");
        return DEREFERENCING_NULL_PTR;
    }

    execution_options exec_opts;
    err_t err = 0;
    String equation_operator = NULL, input_operator = NULL,
           output_operator = NULL;

    if (cli_opts->input_file == NULL) {
        log_fatal("no input file specified");
        return NOT_ENOUTH_ARGUMENTS;
    }
    if (cli_opts->base_output < 2 || cli_opts->base_output > 36) {
        cli_opts->base_output = 10;
    }
    if (cli_opts->base_input < 2 || cli_opts->base_input > 36) {
        cli_opts->base_input = 10;
    }
    if (cli_opts->base_assign < 2 || cli_opts->base_assign > 36) {
        cli_opts->base_assign = 10;
    }

    exec_opts.eq_t = left_eq;
    exec_opts.ex_t = prefix;
    err = hash_table_init(&exec_opts.operators, operators_string_comparer,
                          djb2_hash, sizeof(String *), sizeof(operator_t),
                          operators_bucket_destructor);
    if (err) {
        log_fatal("failed to allocate hash_table");
        return err;
    }

    err = create_ht_with_operators(exec_opts.operators);
    if (err) {
        hash_table_free(exec_opts.operators);
        return err;
    }
    if (cli_opts->config_file == NULL) {
        cli_opts->config_file = fopen("/etc/fuse/fuseconfig", "r");
        if (cli_opts->config_file == NULL) {
            log_fatal("error opening the file");
            hash_table_free(exec_opts.operators);
            return OPENING_THE_FILE_ERROR;
        }
    }

    err =
        parse_config_file(cli_opts->config_file, &exec_opts, &equation_operator,
                          &input_operator, &output_operator);
    if (err) {
        hash_table_free(exec_opts.operators);
        return err;
    }

    err = execution_start(cli_opts, &exec_opts, equation_operator,
                          input_operator, output_operator);
    if (err) {
        hash_table_free(exec_opts.operators);
        return err;
    }

    hash_table_free(exec_opts.operators);
    string_free(equation_operator);
    string_free(input_operator);
    string_free(output_operator);

    return EXIT_SUCCESS;
}

err_t create_ht_with_operators(hash_table *operators) {
    if (operators == NULL) {
        log_fatal("passed ptr is NULL");
        return DEREFERENCING_NULL_PTR;
    }

    err_t err = 0;
    String representation = NULL;

    struct {
        const char *representation;
        operator_type type;
        int priority;
        err_t (*func)(int *, ...);
    } operator_definitions[] = {
        {"add", binary, 4, fuse_add},   {"sub", binary, 4, fuse_sub},
        {"mult", binary, 5, fuse_mult}, {"div", binary, 5, fuse_div},
        {"rem", binary, 5, fuse_rem},   {"pow", binary, 6, fuse_pow},
        {"and", binary, 2, fuse_and},   {"or", binary, 0, fuse_or},
        {"not", unary, 3, fuse_not},    {"xor", binary, 1, fuse_xor},
        {"input", unary, -1, NULL},     {"output", unary, -1, NULL},
        {"=", binary, -1, NULL},        {"(", unary, -2, NULL}};

    size_t operator_count =
        sizeof(operator_definitions) / sizeof(operator_definitions[0]);

    for (size_t i = 0; i < operator_count; ++i) {
        operator_t *op = (operator_t *)malloc(sizeof(operator_t));
        if (op == NULL) {
            log_error("Memory allocation for operator '%s' failed",
                      operator_definitions[i].representation);
            return MEMORY_ALLOCATION_ERROR;
        }

        op->type = operator_definitions[i].type;
        op->priority = operator_definitions[i].priority;
        op->func = operator_definitions[i].func;

        representation = string_from(operator_definitions[i].representation);
        if (representation == NULL) {
            log_error(
                "Failed to allocate memory for operator representation '%s'",
                operator_definitions[i].representation);
            free(op);
            return MEMORY_ALLOCATION_ERROR;
        }

        err = hash_table_set(operators, &representation, op);
        if (err) {
            free(op);
            string_free(representation);
            log_error("Failed to set operator '%s' to hash table",
                      operator_definitions[i].representation);
            return err;
        }

        free(op);
        op = NULL;
    }

    return EXIT_SUCCESS;
}

err_t create_ht_with_real_names(hash_table *names) {
    if (names == NULL) {
        log_fatal("passed ptr is NULL");
        return DEREFERENCING_NULL_PTR;
    }

    err_t err = 0;
    int i = 0;
    String old = NULL, new = NULL;

    char *operator_definitions[] = {"add",   "mult", "rem",    "and", "not",
                                    "input", "=",    "sub",    "div", "pow",
                                    "or",    "xor",  "output", "("};

    size_t operator_count =
        sizeof(operator_definitions) / sizeof(operator_definitions[0]);

    for (i = 0; i < operator_count; ++i) {
        old = string_from(operator_definitions[i]);
        if (old == NULL) {
            log_fatal("failed to init string");
            return MEMORY_ALLOCATION_ERROR;
        }
        new = string_from(operator_definitions[i]);
        if (new == NULL) {
            log_fatal("failed to init string");
            string_free(old);
            return MEMORY_ALLOCATION_ERROR;
        }
        err = hash_table_set(names, &old, &new);
        if (err) {
            log_fatal("failed to set to hash table");
            string_free(old);
            string_free(new);
            return err;
        }

        old = NULL;
        new = NULL;
    }

    return EXIT_SUCCESS;
}

err_t execution_start(CLIOptions *cli_opts, execution_options *exec_opts,
                      String equation_operator, String input_operator,
                      String output_operator) {
    if (cli_opts == NULL || exec_opts == NULL) {
        log_fatal("passed ptr is NULL");
        return DEREFERENCING_NULL_PTR;
    }

    char line[BUFSIZ];
    // trie *variables = NULL;  // TODO !!1!!1!1!!!1!
    hash_table *variables = NULL;
    char current;
    char prev = 'a';
    String instruction = NULL;
    err_t err = 0;
    int i = 0;
    int comment_value = 0;

    // err = trie_init(
    //     &variables,
    //     "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890_");
    // if (err) {
    //     return err;
    // }

    err = hash_table_init(&variables, operators_string_comparer, djb2_hash,
                          sizeof(String *), sizeof(int),
                          operators_bucket_destructor);
    if (err) {
        log_fatal("failed to init hash_table");
        return err;
    }

    instruction = string_init();
    if (instruction == NULL) {
        log_fatal("failed to allocate memory");
        // trie_free(variables);
        hash_table_free(variables);
        return MEMORY_ALLOCATION_ERROR;
    }

    // while (fgets(line, sizeof(line), cli_opts->input_file)) {
    while (!feof(cli_opts->input_file)) {
        current = fgetc(cli_opts->input_file);
        // if (strncmp(line, "#BREAKPOINT", strlen("#BREAKPOINT"))) {
        //     // TODO: start debugging
        // }
        if (current == ';') {
            err = parse_instruction(instruction, cli_opts, exec_opts, variables,
                                    equation_operator, input_operator,
                                    output_operator);
            if (err) {
                string_free(instruction);
                // trie_free(variables);
                hash_table_free(variables);
                return err;
            }
            string_free(instruction);
            instruction = NULL;
            instruction = string_init();
            if (instruction == NULL) {
                log_fatal("failed to allocate memory");
                // trie_free(variables);
                hash_table_free(variables);
                return MEMORY_ALLOCATION_ERROR;
            }
        } else {
            if (current == '[') {
                comment_value++;
                prev = current;
                current++;
                continue;
            }
            if (current == ']') {
                comment_value--;
                if (comment_value < 0) {
                    log_fatal("UJASHO");
                    return INVALID_SYNTAX;
                }
                prev = current;
                current++;
                continue;
            }

            if (comment_value == 0 &&
                (!isspace_c(current) || !isspace_c(prev))) {
                if (current == '#') {
                    break;
                }
                if (isspace_c(current)) {
                    if (string_len(instruction) == 0) {
                        prev = current;
                        continue;
                    }
                    current = ' ';
                }
                err = string_add(&instruction, current);
                if (err) {
                    log_fatal("failed add to string");
                    string_free(instruction);
                    // trie_free(variables);
                    hash_table_free(variables);
                    return err;
                }
            }
        }
        prev = current;
    }
    if (string_len(instruction) > 0) {
        // for (i = 0; i < string_len(instruction); ++i) {
        //     if (!isspace_c(instruction[i])) {
        log_fatal("invalid syntax occured: %s", instruction);
        // trie_free(variables);
        hash_table_free(variables);
        string_free(instruction);
        return INVALID_SYNTAX;
        // }
        // }
    }

    if (comment_value != 0) {
        log_fatal("invalid multi-line commentary");
        string_free(instruction);
        // trie_free(variables);
        hash_table_free(variables);
        return INVALID_SYNTAX;
    }

    string_free(instruction);
    hash_table_free(variables);

    return EXIT_SUCCESS;
}

err_t parse_instruction(String instruction, CLIOptions *cli_opts,
                        execution_options *exec_opts, hash_table *variables,
                        String equartion_operator, String input_operator,
                        String output_operator) {
    if (instruction == NULL || cli_opts == NULL || exec_opts == NULL ||
        variables == NULL) {
        log_fatal("passed ptr is NULL");
        return DEREFERENCING_NULL_PTR;
    }

    String lvalue = NULL, rvalue = NULL;
    int i = 0;
    char current = 0;
    String token = NULL;
    err_t err = 0;
    size_t token_len = 0, instruction_len = 0;
    String tmp = NULL;
    int parsed = 0;

    token = string_init();
    if (token == NULL) {
        log_fatal("failed to allocate memory");
        return MEMORY_ALLOCATION_ERROR;
    }
    err = string_add(&instruction, ' ');

    lvalue = string_init();
    if (lvalue == NULL) {
        log_fatal("failed to allocate memory");
        string_free(token);
        return MEMORY_ALLOCATION_ERROR;
    }
    rvalue = string_init();
    if (rvalue == NULL) {
        log_fatal("failed to allocate memory");
        string_free(lvalue);
        string_free(token);
        return MEMORY_ALLOCATION_ERROR;
    }

    for (i = 0; i < string_len(instruction); ++i) {
        current = instruction[i];
        if (isspace(current)) {
            if (string_cmp(token, equartion_operator) == 0) {
                token_len = string_len(token);
                err = string_grow(&lvalue, i - token_len - 1);
                if (err) {
                    log_fatal("failed to grow string");
                    string_free(lvalue);
                    string_free(token);
                    return err;
                }
                memcpy(lvalue, instruction, i - token_len - 1);
                __cstring_string_to_base(lvalue)->length = i - token_len - 1;

                instruction_len = string_len(instruction);
                err = string_grow(&rvalue, instruction_len - i - token_len + 1);
                if (err) {
                    log_fatal("failed to grow string");
                    string_free(lvalue);
                    string_free(token);
                    return err;
                }
                memcpy(rvalue, instruction + i + token_len - 1,
                       instruction_len - i - token_len + 1);
                __cstring_string_to_base(rvalue)->length =
                    instruction_len - i - token_len + 1;

                if (exec_opts->eq_t == right_eq) {
                    tmp = lvalue;
                    lvalue = rvalue;
                    rvalue = tmp;
                    tmp = NULL;
                }
                parsed = 1;
                err = parse_expression(lvalue, rvalue, cli_opts, exec_opts,
                                       variables);
                if (err) {
                    string_free(lvalue);
                    string_free(token);
                    return err;
                }
            }

            if (string_cmp(token, output_operator) == 0) {
                parsed = 1;
                err =
                    handle_output(instruction, cli_opts, exec_opts, variables);
                if (err) {
                    string_free(token);
                    return err;
                }
            } else if (string_cmp(token, input_operator) == 0) {
                parsed = 1;
                err = handle_input(instruction, cli_opts, exec_opts, variables);
                if (err) {
                    string_free(token);
                    return err;
                }
            }

            // if (!parsed && string_len(instruction) > 0) {
            //     log_fatal("invalid instruction syntax: %s", instruction);
            //     string_free(lvalue);
            //     string_free(rvalue);
            //     string_free(token);
            //     return INVALID_SYNTAX;
            // }  // mb REDO

            string_free(token);
            token = NULL;
            token = string_init();
            if (token == NULL) {
                log_fatal("failed to allocate memory");
                string_free(lvalue);
                string_free(rvalue);
                return MEMORY_ALLOCATION_ERROR;
            }

        } else {
            err = string_add(&token, current);
            if (err) {
                log_fatal("failed to add to string");
                string_free(lvalue);
                string_free(rvalue);
                string_free(token);
                return err;
            }
        }
    }

    string_free(lvalue);
    string_free(rvalue);
    string_free(token);

    return EXIT_SUCCESS;
}

err_t parse_expression(String lvalue, String rvalue, CLIOptions *cli_opts,
                       execution_options *exec_opts, hash_table *variables) {
    if (lvalue == NULL || rvalue == NULL || cli_opts == NULL ||
        exec_opts == NULL || variables == NULL) {
        log_fatal("passed ptr is NULL");
        return DEREFERENCING_NULL_PTR;
    }

    err_t err = 0;

    switch (exec_opts->ex_t) {
        case prefix:
            err = execute_prefix_expression(lvalue, rvalue, cli_opts, exec_opts,
                                            variables);
            if (err) {
                return err;
            }
            break;
        case infix:
            err = execute_infix_expression(lvalue, rvalue, cli_opts, exec_opts,
                                           variables);
            if (err) {
                return err;
            }
            break;
        case postfix:
            err = execute_postfix_expression(lvalue, rvalue, cli_opts,
                                             exec_opts, variables);
            if (err) {
                return err;
            }
            break;
    }

    return EXIT_SUCCESS;
}

int is_operand(int c) {
    if (isalnum(c) || c == '_') {
        return 1;
    }
    return 0;
}

err_t handle_input(String instruction, CLIOptions *cli_opts,
                   execution_options *exec_opts, hash_table *variables) {
    if (instruction == NULL || cli_opts == NULL || exec_opts == NULL ||
        variables == NULL) {
        log_fatal("passed ptr is NULL");
    }

    int i = 0;
    char current = 0;
    String token = NULL;
    int *value_placeholder;
    err_t err = 0;
    char user_ans[BUFSIZ];
    int result;

    token = string_init();
    if (token == NULL) {
        log_fatal("failed to alloc memor");
        return MEMORY_ALLOCATION_ERROR;
    }
    for (i = 0; i < string_len(instruction); ++i) {
        if (isspace_c(instruction[i])) {
            break;
        }
    }
    for (i = i + 1; i < string_len(instruction); ++i) {
        if (isspace_c(instruction[i])) {
            break;
        }
        err = string_add(&token, instruction[i]);
        if (err) {
            log_fatal("failed add to string");
            string_free(token);
            return err;
        }
    }
    for (i = i + 1; i < string_len(instruction); ++i) {
        if (!isspace_c(instruction[i])) {
            log_fatal("invalid instruction: %s", instruction);
            string_free(token);
            return INVALID_SYNTAX;
        }
    }

    err = string_add(&token, '\0');
    if (err) {
        log_fatal("failed to add to string");
        string_free(token);
        return err;
    }
    __cstring_string_to_base(token)->length--;

    printf("Enter value for %s variable in %d counting system: ", token,
           cli_opts->base_input);
    scanf("%s", user_ans);
    err = catoi(user_ans, cli_opts->base_input, &result);
    if (err) {
        log_fatal("failed to parse string to int");
        string_free(token);
        return err;
    }

    err = hash_table_set(variables, &token, &result);
    if (err) {
        log_fatal("failed to set to hash table");
        string_free(token);
        return err;
    }

    return EXIT_SUCCESS;
}
err_t handle_output(String instruction, CLIOptions *cli_opts,
                    execution_options *exec_opts, hash_table *variables) {
    if (instruction == NULL || cli_opts == NULL || exec_opts == NULL ||
        variables == NULL) {
        log_fatal("passed ptr is NULL");
    }
    int i = 0;
    char current = 0;
    String token = NULL;
    int *value_placeholder;
    err_t err = 0;
    char *to_output = NULL;

    token = string_init();
    if (token == NULL) {
        log_fatal("failed to alloc memor");
        return MEMORY_ALLOCATION_ERROR;
    }
    for (i = 0; i < string_len(instruction); ++i) {
        if (isspace_c(instruction[i])) {
            break;
        }
    }
    for (i = i + 1; i < string_len(instruction); ++i) {
        if (isspace_c(instruction[i])) {
            break;
        }
        err = string_add(&token, instruction[i]);
        if (err) {
            log_fatal("failed add to string");
            string_free(token);
            return err;
        }
    }

    for (i = i + 1; i < string_len(instruction); ++i) {
        if (!isspace_c(instruction[i])) {
            log_fatal("invalid instruction: %s", instruction);
            string_free(token);
            return INVALID_SYNTAX;
        }
    }

    err = hash_table_get(variables, &token, (void **)&value_placeholder);
    if (err != EXIT_SUCCESS && err != KEY_NOT_FOUND) {
        log_fatal("failed to get from ht");
        string_free(token);
        return err;
    }
    if (err == KEY_NOT_FOUND) {
        log_fatal("Invalid output variable: %s", token);
        string_free(token);
        return INVALID_SYNTAX;
    }
    err = citoa(*value_placeholder, cli_opts->base_output, &to_output);
    if (err) {
        log_fatal("failed to convert number to string");
        string_free(token);
        return INVALID_SYNTAX;
    }

    string_print(token);
    printf(" variable value in %d counting system: %s\n", cli_opts->base_output,
           to_output);

    string_free(token);
    free(to_output);

    return EXIT_SUCCESS;
}
