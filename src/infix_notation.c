#include "infix_notation.h"

#include <ctype.h>
#include <stdlib.h>

#include "../include/logger.h"
#include "../include/stack.h"
#include "fuse.h"
#include "postfix_notation.h"

void infix_notation_string_free(void *s) {  // for stack in infix_to_postfix
    String *st = s;
    string_free(*st);
    free(st);
    return;
}

err_t infix_to_postfix(const String infix_exp, int (*is_operand)(int c),
                       hash_table *operators_ht, String *postfix_exp) {
    if (infix_exp == NULL || is_operand == NULL || operators_ht == NULL ||
        postfix_exp == NULL) {
        log_error("passed NULL ptr");
        return DEREFERENCING_NULL_PTR;
    }

    stack *operators = NULL;
    stack_item *p_for_stack = NULL;
    err_t err = 0;
    size_t i = 0, j = 0, k = 0;
    char ie = 0;
    char *current_p = NULL;
    String buffer = NULL,
           for_stack = NULL;  // Buffer for multi-character operators
    size_t buffer_len = 0, exp_len = string_len(infix_exp);
    operator_t *op1 = NULL, *op2 = NULL;
    String for_ht = NULL;
    char tmp;

    err = stack_init(&operators, sizeof(String *), infix_notation_string_free);
    if (err) {
        log_error("failed to initialize stack");
        return err;
    }

    buffer = string_from("(");
    if (buffer == NULL) {
        stack_free(operators);
        log_error("Failed to allocate memory for buffer");
        return MEMORY_ALLOCATION_ERROR;
    }
    err = stack_push(operators, &buffer);
    if (err) {
        log_error("failed to push to stack");
        string_free(buffer);
        stack_free(operators);
        return err;
    }
    buffer = NULL;

    for (i = 0; i < exp_len; ++i) {
        current_p = infix_exp + i;
        ie = infix_exp[i];

        for_ht = string_init();
        for (k = i; k < exp_len; ++k) {
            tmp = infix_exp[k];
            if (isspace(tmp) || tmp == '(' || tmp == ')' || tmp == ',') {
                break;
            } else {
                err = string_add(&for_ht, tmp);
                if (err) {
                    log_fatal("failed to add to string");
                    string_free(buffer);
                    string_free(for_ht);
                    stack_free(operators);
                    return err;
                }
            }
        }
        err = hash_table_get(operators_ht, &for_ht, (void **)&op1);
        if (err != EXIT_SUCCESS && err != KEY_NOT_FOUND) {
            log_error("failed to get from hash table");
            string_free(buffer);
            string_free(for_ht);
            stack_free(operators);
            return err;
        }
        if (err == KEY_NOT_FOUND) {
            buffer_len = 0;
        } else {
            buffer_len = string_len(for_ht);
        }
        string_free(for_ht);

        if (buffer_len > 0) {  // found operator

            buffer = string_init();
            if (buffer == NULL) {
                stack_free(operators);
                log_error("Failed to allocate memory for buffer");
                return MEMORY_ALLOCATION_ERROR;
            }
            for (j = 0; j < buffer_len; ++j) {
                err = string_add(&buffer, current_p[j]);
                if (err) {
                    log_error("failed push to string");
                    stack_free(operators);
                    string_free(buffer);
                    return err;
                }
                i++;
            }
            i--;
            ie = infix_exp[i];
            current_p = infix_exp + i;

            while (1) {
                err = stack_top(operators, &p_for_stack);
                if (err != EXIT_SUCCESS && err != STACK_IS_EMPTY) {
                    log_error("top from stack failed: %d", err);
                    stack_free(operators);
                    string_free(buffer);
                    return err;
                }

                if (err == STACK_IS_EMPTY) {
                    log_error("stack_is_empty, invalid braces placement");
                    stack_free(operators);
                    string_free(buffer);
                    return INVALID_BRACES;
                }

                for_stack = *(String *)p_for_stack->data;
                // if (priority_mapper(for_stack) < priority_mapper(buffer)) {
                //     break;
                // } // TODO: redo !!!!1!!

                err = hash_table_get(operators_ht, &for_stack, (void **)&op1);
                err = hash_table_get(operators_ht, &buffer, (void **)&op2);

                if (op1->priority < op2->priority) {
                    break;
                }

                err = string_cat(postfix_exp, &for_stack);
                if (err) {
                    log_error("failed to cat to the string");
                    stack_free(operators);
                    string_free(buffer);
                    return err;
                }

                err = string_add(postfix_exp, ' ');
                if (err) {
                    log_error("failed to push to the string");
                    stack_free(operators);
                    string_free(buffer);
                    return err;
                }
                err = stack_pop(operators);
                if (err != EXIT_SUCCESS && err != STACK_IS_EMPTY) {
                    log_error("pop from stack failed");
                    stack_free(operators);
                    return err;
                }
                for_stack = NULL;
                if (err == STACK_IS_EMPTY) {
                    log_error("stack is empty");
                    stack_free(operators);
                    return INVALID_BRACES;
                }
            }
            err = stack_push(operators, &buffer);
            if (err) {
                log_error("failed push to stack");
                stack_free(operators);
                string_free(buffer);
                return err;
            }

            buffer = NULL;
            continue;
        } else if (ie == '(') {
            buffer = string_from("(");
            if (buffer == NULL) {
                stack_free(operators);
                log_error("Failed to allocate memory for buffer");
                return MEMORY_ALLOCATION_ERROR;
            }
            err = stack_push(operators, &buffer);
            if (err) {
                log_error("failed to push to stack");
                string_free(buffer);
                stack_free(operators);
                return err;
            }
            buffer = NULL;
        } else if (ie == ')') {
            while (1) {
                err = stack_top(operators, &p_for_stack);
                if (err != EXIT_SUCCESS && err != STACK_IS_EMPTY) {
                    log_error("top from stack failed");
                    stack_free(operators);
                    return err;
                }
                if (err == STACK_IS_EMPTY) {
                    log_error("stack_is_empty");
                    stack_free(operators);
                    return INVALID_BRACES;
                }
                buffer = *(String *)p_for_stack->data;

                if (buffer[0] == '(') {
                    err = stack_pop(operators);
                    if (err) {
                        log_error("failed pop from stack");
                        stack_free(operators);
                        return err;
                    }
                    break;
                }
                err = string_cat(postfix_exp, &buffer);
                if (err) {
                    log_error("failed to cat to the string");
                    stack_free(operators);
                    return err;
                }
                err = string_add(postfix_exp, ' ');
                if (err) {
                    log_error("failed to push to the string");
                    stack_free(operators);
                    return err;
                }

                err = stack_pop(operators);
                if (err) {
                    log_error("failed pop from stack");
                    stack_free(operators);
                    return err;
                }
            }
        } else if (is_operand(ie)) {
            while (is_operand(ie) && i < exp_len) {
                err = string_add(postfix_exp, ie);
                if (err) {
                    log_error("failed to push to the string");
                    stack_free(operators);
                    return err;
                }
                i++;
                ie = infix_exp[i];
            }
            string_add(postfix_exp, ' ');
            if (err) {
                log_error("failed to push to the string");
                stack_free(operators);
                return err;
            }
            i--;
            ie = infix_exp[i];
            continue;
        } else if (ie != ' ') {
            log_error("invalid symbol '%c' found", ie);
            stack_free(operators);
            return INVALID_SYMBOL;
        }
    }

    while (1) {
        err = stack_top(operators, &p_for_stack);
        if (err != EXIT_SUCCESS && err != STACK_IS_EMPTY) {
            log_error("top from stack failed");
            stack_free(operators);
            return err;
        }
        if (err == STACK_IS_EMPTY) {
            log_error("stack is empty, invalid braces placement");
            stack_free(operators);
            return INVALID_BRACES;
        }
        buffer = *(String *)p_for_stack->data;

        if (buffer[0] == '(') {
            err = stack_pop(operators);
            if (err) {
                log_error("failed pop from stack");
                stack_free(operators);
                return err;
            }
            break;
        }
        err = string_cat(postfix_exp, &buffer);
        if (err) {
            log_error("failed to cat to the string");
            stack_free(operators);
            return err;
        }
        err = string_add(postfix_exp, ' ');
        if (err) {
            log_error("failed to cat to the string");
            stack_free(operators);
            return err;
        }
        err = stack_pop(operators);
        if (err) {
            log_error("failed pop from stack");
            stack_free(operators);
            return err;
        }
    }

    if (!stack_is_empty(operators)) {
        log_error(
            "transformation done, stack is not emtpy, invalid braces, %zu",
            operators->size);
        stack_free(operators);
        return INVALID_BRACES;
    }

    exp_len = string_len(*postfix_exp);

    if (exp_len > 0 && (*postfix_exp)[exp_len - 1] == ' ') {
        string_grow(postfix_exp, exp_len);
    }

    stack_free(operators);
    return EXIT_SUCCESS;
}

err_t execute_infix_expression(String lvalue, String rvalue,
                               CLIOptions *cli_opts,
                               execution_options *exec_opts,
                               hash_table *variables) {
    if (lvalue == NULL || rvalue == NULL || cli_opts == NULL ||
        exec_opts == NULL || variables == NULL) {
        log_fatal("passed ptr is NULL");
        return DEREFERENCING_NULL_PTR;
    }

    String postfix_exp = NULL;
    err_t err = 0;
    int result = 0, i = 0;
    String for_ht;

    postfix_exp = string_init();
    if (postfix_exp == NULL) {
        log_fatal("failed to allocate memory");
        return MEMORY_ALLOCATION_ERROR;
    }

    err = infix_to_postfix(rvalue, is_operand, exec_opts->operators,
                           &postfix_exp);
    if (err) {
        string_free(postfix_exp);
        return err;
    }

    err = calculate_postfix_expression(postfix_exp, &result,
                                       exec_opts->operators, variables);
    if (err) {
        string_free(postfix_exp);
        return err;
    }

    for (i = 0; i < string_len(lvalue); ++i) {
        if (!is_operand(lvalue[i])) {
            log_error("invalid lvalue: %s", lvalue);
            string_free(postfix_exp);
            return INVALID_SYNTAX;
        }
    }
    for (i = 0; i < string_len(lvalue); ++i) {
        if (!is_operand(lvalue[i])) {
            log_error("invalid lvalue: %s", lvalue);
            return INVALID_SYNTAX;
        }
    }

    for_ht = string_init();
    string_cpy(&for_ht, &lvalue);
    // TODO: errors

    err = hash_table_set(variables, &for_ht, &result);
    if (err) {
        log_fatal("failed to set to hash table");
        string_free(for_ht);
        string_free(postfix_exp);
        return err;
    }

    string_free(postfix_exp);

    return EXIT_SUCCESS;
}
