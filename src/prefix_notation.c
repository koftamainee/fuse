#include "prefix_notation.h"

#include <ctype.h>
#include <stdlib.h>

#include "../include/logger.h"
#include "../include/stack.h"
#include "../include/types.h"
#include "fuse.h"
#include "postfix_notation.h"

void prefix_notation_string_destructor(void *s) {
    String *str = s;
    string_free(*str);
    free(str);
}

err_t prefix_to_postfix(const String prefix_exp, int (*is_operand)(int c),
                        hash_table *operators_ht, String *postfix_exp) {
    if (prefix_exp == NULL || is_operand == NULL || operators_ht == NULL ||
        postfix_exp == NULL) {
        log_error("passed NULL ptr");
        return DEREFERENCING_NULL_PTR;
    }

    stack *st = NULL;
    String reversed = NULL;
    err_t err = 0;
    int i = 0, j = 0;
    char pe = 0;
    String token = NULL;
    operator_t *op = NULL;
    String operand_1 = NULL, operand_2 = NULL;
    stack_item *for_stack = NULL;
    String result = NULL;

    err = stack_init(&st, sizeof(String *), prefix_notation_string_destructor);
    if (err) {
        log_fatal("failed to init stack");
        string_free(result);
        return err;
    }

    reversed = string_from(" ");  // space in the start for proper handling
    if (reversed == NULL) {
        log_fatal("failed to allocate memory");
        stack_free(st);
        string_free(result);
        return MEMORY_ALLOCATION_ERROR;
    }
    err = string_cat(&reversed, &prefix_exp);
    if (err) {
        log_fatal("failed to cat to string");
        stack_free(st);
        string_free(result);
        return err;
    }
    token = string_init();
    if (token == NULL) {
        log_fatal("failed to allocate memory");

        string_free(result);
        stack_free(st);
        string_free(reversed);
        return MEMORY_ALLOCATION_ERROR;
    }
    for (i = string_len(reversed) - 1; i >= 0; --i) {
        pe = reversed[i];
        if (isspace_c(pe)) {
            if (string_len(token) == 0) {
                continue;
            }
            err = string_reverse(token);
            if (err) {
                log_error("failed to reverse string");
                stack_free(st);
                string_free(reversed);
                string_free(token);
                string_free(result);
                return err;
            }

            err = hash_table_get(operators_ht, &token, (void **)&op);
            if (err != EXIT_SUCCESS && err != KEY_NOT_FOUND) {
                log_fatal("failed to get from hash table");
                stack_free(st);
                string_free(reversed);
                string_free(token);
                string_free(result);
                return err;
            }
            if (err == KEY_NOT_FOUND) {  // operand
                for (j = 0; j < string_len(token); ++j) {
                    if (!is_operand(token[j])) {
                        log_fatal("invalid operand found: \"%s\"", token);
                        stack_free(st);
                        string_free(reversed);
                        string_free(token);
                        string_free(result);
                        return INVALID_SYNTAX;
                    }
                }
                err = stack_push(st, &token);
                if (err) {
                    log_fatal("failed to push to stack");
                    stack_free(st);
                    string_free(reversed);
                    string_free(token);
                    string_free(result);
                    return err;
                }
                token = NULL;
            } else {  // operator
                result = string_init();
                if (result == NULL) {
                    log_fatal("failed to allocate memory");
                    stack_free(st);
                    string_free(reversed);
                    string_free(token);
                    string_free(result);
                    return MEMORY_ALLOCATION_ERROR;
                }
                if (op->type == binary) {
                    err = stack_top(st, &for_stack);
                    if (err) {
                        log_fatal("failed top from stack. Invalid expression");
                        stack_free(st);
                        string_free(reversed);
                        string_free(token);
                        string_free(result);
                        return err;
                    }
                    operand_1 = string_clone(*(String *)for_stack->data);
                    if (operand_1 == NULL) {
                        log_fatal("failed to allocate memory");
                        stack_free(st);
                        string_free(reversed);
                        string_free(token);
                        string_free(result);
                        return MEMORY_ALLOCATION_ERROR;
                    }
                    err = stack_pop(st);
                    if (err) {
                        log_fatal("failed pop from stack");
                        stack_free(st);
                        string_free(reversed);
                        string_free(token);
                        string_free(result);
                        return err;
                    }
                    err = stack_top(st, &for_stack);
                    if (err) {
                        log_fatal("failed top from stack. Invalid expression");
                        stack_free(st);
                        string_free(reversed);
                        string_free(token);
                        string_free(result);
                        return err;
                    }
                    operand_2 = string_clone(*(String *)for_stack->data);
                    if (operand_2 == NULL) {
                        log_fatal("failed to allocate memory");
                        stack_free(st);
                        string_free(reversed);
                        string_free(token);
                        string_free(result);
                        return MEMORY_ALLOCATION_ERROR;
                    }
                    err = stack_pop(st);
                    if (err) {
                        log_fatal("failed pop from stack");
                        stack_free(st);
                        string_free(reversed);
                        string_free(token);
                        string_free(result);
                        return err;
                    }

                    err = string_cat(&result, &operand_1);
                    if (err) {
                        log_fatal("failed to cat to string");
                        stack_free(st);
                        string_free(reversed);
                        string_free(token);
                        string_free(result);
                        return err;
                    }
                    string_free(operand_1);
                    operand_1 = NULL;
                    err = string_add(&result, ' ');
                    if (err) {
                        log_fatal("failed to add to string");
                        stack_free(st);
                        string_free(reversed);
                        string_free(token);
                        string_free(result);
                        return err;
                    }
                    err = string_cat(&result, &operand_2);
                    if (err) {
                        log_fatal("failed to cat to string");
                        stack_free(st);
                        string_free(reversed);
                        string_free(token);
                        string_free(result);
                        return err;
                    }
                    string_free(operand_2);
                    operand_2 = NULL;
                    err = string_add(&result, ' ');
                    if (err) {
                        log_fatal("failed to add to string");
                        stack_free(st);
                        string_free(reversed);
                        string_free(token);
                        string_free(result);
                        return err;
                    }
                    err = string_cat(&result, &token);
                    if (err) {
                        log_fatal("failed to cat to string");
                        stack_free(st);
                        string_free(reversed);
                        string_free(token);
                        string_free(result);
                        return err;
                    }
                    err = string_add(&result, ' ');
                    if (err) {
                        log_fatal("failed to add to string");
                        stack_free(st);
                        string_free(reversed);
                        string_free(token);
                        string_free(result);
                        return err;
                    }
                    err = stack_push(st, &result);
                    if (err) {
                        log_fatal("failed push to the stack");
                        stack_free(st);
                        string_free(reversed);
                        string_free(token);
                        string_free(result);
                        return err;
                    }
                    result = NULL;

                } else {
                    err = stack_top(st, &for_stack);
                    if (err) {
                        log_fatal("failed top from stack. Invalid expression");
                        stack_free(st);
                        string_free(reversed);
                        string_free(token);
                        string_free(result);
                        return err;
                    }
                    operand_1 = string_clone(*(String *)for_stack->data);
                    if (operand_1 == NULL) {
                        log_fatal("failed to allocate memory");
                        stack_free(st);
                        string_free(reversed);
                        string_free(token);
                        string_free(result);
                        return MEMORY_ALLOCATION_ERROR;
                    }
                    err = stack_pop(st);
                    if (err) {
                        log_fatal("failed pop from stack");
                        stack_free(st);
                        string_free(reversed);
                        string_free(token);
                        string_free(result);
                        return err;
                    }
                    err = string_cat(&result, &operand_1);
                    if (err) {
                        log_fatal("failed to cat to string");
                        stack_free(st);
                        string_free(reversed);
                        string_free(token);
                        string_free(result);
                        return err;
                    }
                    string_free(operand_1);
                    operand_1 = NULL;
                    err = string_add(&result, ' ');
                    if (err) {
                        log_fatal("failed to add to string");
                        stack_free(st);
                        string_free(reversed);
                        string_free(token);
                        string_free(result);
                        return err;
                    }
                    err = string_cat(&result, &token);
                    if (err) {
                        log_fatal("failed to cat to string");
                        stack_free(st);
                        string_free(reversed);
                        string_free(token);
                        string_free(result);
                        return err;
                    }
                    err = string_add(&result, ' ');
                    if (err) {
                        log_fatal("failed to add to string");
                        stack_free(st);
                        string_free(reversed);
                        string_free(token);
                        string_free(result);
                        return err;
                    }
                    err = stack_push(st, &result);
                    if (err) {
                        log_fatal("failed push to the stack");
                        stack_free(st);
                        string_free(reversed);
                        string_free(token);
                        string_free(result);
                        return err;
                    }
                    result = NULL;
                }
            }

            string_free(token);
            token = NULL;
            token = string_init();
            if (token == NULL) {
                log_fatal("failed to allocate memory");
                stack_free(st);
                string_free(reversed);
                string_free(token);
                return MEMORY_ALLOCATION_ERROR;
            }
        } else {
            err = string_add(&token, pe);
            if (err) {
                log_fatal("failed to add to string");
                stack_free(st);
                string_free(reversed);
                string_free(token);
                string_free(result);
                return err;
            }
        }
    }

    err = stack_top(st, &for_stack);
    if (err) {
        log_fatal("failed top from stack");
        stack_free(st);
        string_free(token);
        string_free(reversed);
        return err;
    }
    if (st->size > 1) {
        log_fatal("invalid expression.");
        stack_free(st);
        string_free(token);
        string_free(reversed);
        return INVALID_SYNTAX;
    }
    *postfix_exp = string_clone(*(String *)for_stack->data);

    stack_free(st);
    string_free(token);
    string_free(reversed);

    return EXIT_SUCCESS;
}

err_t execute_prefix_expression(String lvalue, String rvalue,
                                CLIOptions *cli_opts,
                                execution_options *exec_opts,
                                hash_table *variables) {
    if (lvalue == NULL || rvalue == NULL || cli_opts == NULL ||
        exec_opts == NULL || variables == NULL) {
        log_fatal("passed ptr is NULL");
        return DEREFERENCING_NULL_PTR;
    }

    String valid_expression = NULL, for_spaces = NULL, for_ht = NULL;
    err_t err = 0;
    int i = 0, result;
    char current = 0, prev = 52;
    String postfix_exp = NULL;

    valid_expression = string_init();
    if (valid_expression == NULL) {
        log_fatal("memory allocation error");
        return MEMORY_ALLOCATION_ERROR;
    }

    for (i = 0; i < string_len(rvalue); ++i) {
        current = rvalue[i];

        if ((current == ',' || current == '(' || current == ')')) {
            if (prev != ' ') {
                if (string_len(valid_expression)) {
                    err = string_add(&valid_expression, ' ');
                    if (err) {
                        log_fatal("failed to add to string");
                        string_free(valid_expression);
                        return err;
                    }
                }
            }
        } else {
            err = string_add(&valid_expression, current);
            if (err) {
                log_fatal("failed to add to string");
                string_free(valid_expression);
                return err;
            }
        }
        prev = current;
    }

    for_spaces = valid_expression;
    valid_expression = NULL;
    valid_expression = string_init();
    if (valid_expression == NULL) {
        log_fatal("failed to allocate memory");
        string_free(for_spaces);
        return MEMORY_ALLOCATION_ERROR;
    }
    prev = ' ';
    for (i = 0; i < string_len(for_spaces); ++i) {
        current = for_spaces[i];
        if (!(isspace(current) && isspace(prev))) {
            err = string_add(&valid_expression, current);
            if (err) {
                log_fatal("failed to add to string");
                string_free(valid_expression);
                return err;
            }
        }
        prev = current;
    }

    string_free(for_spaces);
    for_spaces = NULL;

    err = string_add(&valid_expression, ' ');
    if (err) {
        log_fatal("failed to add to string");
        string_free(valid_expression);
        return err;
    }
    err = prefix_to_postfix(valid_expression, is_operand, exec_opts->operators,
                            &postfix_exp);
    if (err) {
        string_free(valid_expression);
        return err;
    }

    err = string_add(&postfix_exp, ' ');
    if (err) {
        string_free(postfix_exp);
        string_free(valid_expression);
        return err;
    }

    err = calculate_postfix_expression(postfix_exp, &result,
                                       exec_opts->operators, variables);
    if (err) {
        string_free(postfix_exp);
        string_free(valid_expression);
        return err;
    }

    for (i = 0; i < string_len(lvalue); ++i) {
        if (!is_operand(lvalue[i])) {
            log_error("invalid lvalue: %s", lvalue);
            string_free(valid_expression);
            return INVALID_SYNTAX;
        }
    }

    for (i = 0; i < string_len(lvalue); ++i) {
        if (!is_operand(lvalue[i])) {
            log_error("invalid lvalue: %s", lvalue);
            string_free(postfix_exp);
            return INVALID_SYNTAX;
        }
    }

    for_ht = string_init();
    err = string_cpy(&for_ht, &lvalue);
    if (err) {
        log_fatal("failed to cpy string");
        string_free(valid_expression);
        string_free(postfix_exp);
        return err;
    }

    err = hash_table_set(variables, &for_ht, &result);
    if (err) {
        log_fatal("failed to set to hash table");
        string_free(for_ht);
        string_free(valid_expression);
        return err;
    }

    string_free(postfix_exp);
    string_free(valid_expression);

    return EXIT_SUCCESS;
}
