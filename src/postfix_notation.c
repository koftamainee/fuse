#include "postfix_notation.h"

#include <ctype.h>
#include <stdlib.h>

#include "../include/logger.h"
#include "../include/stack.h"
#include "../include/types.h"

err_t execute_postfix_expression(String lvalue, String rvalue,
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

    err = calculate_postfix_expression(valid_expression, &result,
                                       exec_opts->operators, variables);
    if (err) {
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
        string_free(valid_expression);
        return err;
    }

    string_free(valid_expression);

    return EXIT_SUCCESS;
}

err_t calculate_postfix_expression(const String postfix_exp,
                                   int *expression_result,
                                   hash_table *operators,
                                   hash_table *operands) {
    if (postfix_exp == NULL || expression_result == NULL || operators == NULL ||
        operands == NULL) {
        log_error("Passed ptr is NULL");
        return DEREFERENCING_NULL_PTR;
    }

    size_t i = 0;
    err_t err = 0;
    char pe = 0;
    String token = NULL, for_hash_table = NULL;
    stack *st = NULL;
    operator_t *op = NULL;
    int operand_1 = 0, operand_2 = 0, result = 0, *get_from_hash_table = NULL;
    stack_item *p_for_stack = NULL;

    err = stack_init(&st, sizeof(int), free);
    if (err) {
        log_error("error during stack initialization");
        return err;
    }

    token = string_init();
    if (token == NULL) {
        log_error("failed to allocate memory for token");
        stack_free(st);
        return MEMORY_ALLOCATION_ERROR;
    }

    for (i = 0; i < string_len(postfix_exp); ++i) {
        pe = postfix_exp[i];
        // border principle
        if (pe == ' ') {
            if (string_len(token) == 0) {
                continue;
            }
            op = NULL;
            err = hash_table_get(operators, &token, (void **)&op);
            if (err != EXIT_SUCCESS && err != KEY_NOT_FOUND) {
                log_error("Error while getting elem from hash table");
                stack_free(st);
                string_free(token);
                return err;
            }

            if (err == KEY_NOT_FOUND) {  // not an operator, is operand

                err = hash_table_get(operands, &token,
                                     (void **)&get_from_hash_table);
                if (err != EXIT_SUCCESS && err != KEY_NOT_FOUND) {
                    log_error("Error while getting elem from trie");
                    stack_free(st);
                    string_free(token);
                    return err;
                } else if (err == EXIT_SUCCESS) {  // is variable
                    operand_1 = *get_from_hash_table;
                    err = stack_push(st, &operand_1);
                    if (err) {
                        log_error("failed push to stack");
                        stack_free(st);
                        string_free(token);
                        return err;
                    }
                    operand_1 = 0;

                } else {  // is digits
                    err = catoi_s(token, 10, &operand_1);
                    if (err) {
                        log_error("invalid variable or numeric system: %s",
                                  token);
                        stack_free(st);
                        string_free(token);
                        return err;
                    }
                    err = stack_push(st, &operand_1);
                    if (err) {
                        log_error("failed push to stack");
                        stack_free(st);
                        string_free(token);
                        return err;
                    }
                    operand_1 = 0;
                }

                // if (isdigit_s(token)) {  // found token is digits, call
                // gorner
                //     err = catoi_s(token, 10, &operand_1);  // and push to
                //     stack if (err) {
                //         log_error("failed to transmit str to int");
                //         stack_free(st);
                //         string_free(token);
                //         return err;
                //     }
                //     err = stack_push(st, &operand_1);
                //     if (err) {
                //         log_error("failed push to stack");
                //         stack_free(st);
                //         string_free(token);
                //         return err;
                //     }
                //     operand_1 = 0;
                // } else {  // token is varialbe, check if it in hash table
                //     err = hash_table_get(operands, &token,
                //                          (void **)&get_from_hash_table);
                //     // err = trie_get(variables, token, &operand_1);
                //     if (err != EXIT_SUCCESS && err != KEY_NOT_FOUND) {
                //         log_error("Error while getting elem from trie");
                //         stack_free(st);
                //         string_free(token);
                //         return err;
                //     }
                //
                //     if (err == KEY_NOT_FOUND) {  // variable not found in
                //     hash
                //                                  // table, asking user
                //                                  for it
                //
                //         // log_fatal("undeclared variable is used: %s",
                //         token);
                //         // stack_free(st);
                //         // string_free(token);
                //         / return err;
                //         printf("Please enter value for '");
                //         string_print(token);
                //         printf("' variable: ");
                //         while (1) {
                //             if (scanf("%d", &operand_1) == 1) {
                //                 break;
                //             } else {
                //                 printf(
                //                     "Invalid input. Please enter a valid
                //                     " "integer.\n");
                //
                //                 while (getchar() != '\n');
                //                 printf("Please try again: ");
                //             }
                //         }
                //
                //         for_hash_table = string_init();
                //         if (for_hash_table == NULL) {
                //             log_error("Error to allocate memory for
                //             string"); stack_free(st); string_free(token);
                //             return err;
                //         }
                //         err = string_cpy(&for_hash_table, &token);
                //         if (err) {
                //             log_error("Error cpy string");
                //             stack_free(st);
                //             string_free(token);
                //             string_free(for_hash_table);
                //             return err;
                //         }
                //
                //         err = hash_table_set(operands, &for_hash_table,
                //                              &operand_1);
                //         // err = trie_set(variables, for_hash_table,
                //         operand_1); if (err) {
                //             log_error("Error push to trie");
                //             stack_free(st);
                //             string_free(token);
                //             return err;
                //         }
                //         for_hash_table = NULL;
                //     }
                //
                //     // pushing variable to stack
                //     err = stack_push(st, &operand_1);
                //     if (err) {
                //         log_error("Error push to stack");
                //         stack_free(st);
                //         string_free(token);
                //         return err;
                //     }
                // }
            } else {  // operator found
                if (op->type == binary) {
                    err = stack_top(st, &p_for_stack);
                    if (err != EXIT_SUCCESS && err != STACK_IS_EMPTY) {
                        log_error("Error top from stack");
                        stack_free(st);
                        string_free(token);
                        return err;
                    }
                    if (err == STACK_IS_EMPTY) {
                        log_error(
                            "stack is empty, not enouth operands for "
                            "operators");
                        stack_free(st);
                        string_free(token);
                        return INVALID_OPERATIONS;
                    }
                    operand_1 = *(int *)p_for_stack->data;
                    err = stack_pop(st);
                    if (err) {
                        log_error("Error pop from stack");
                        stack_free(st);
                        string_free(token);
                        return err;
                    }

                    err = stack_top(st, &p_for_stack);
                    if (err != EXIT_SUCCESS && err != STACK_IS_EMPTY) {
                        log_error("Error top from stack");
                        stack_free(st);
                        string_free(token);
                        return err;
                    }
                    if (err == STACK_IS_EMPTY) {
                        log_error(
                            "stack is empty, not enouth operands for "
                            "operators");
                        stack_free(st);
                        string_free(token);
                        return INVALID_OPERATIONS;
                    }
                    operand_2 = *(int *)p_for_stack->data;

                    err = stack_pop(st);
                    if (err) {
                        log_error("Error pop from stack");
                        stack_free(st);
                        string_free(token);
                        return err;
                    }

                    // result = op->func(operand_2,
                    //                   operand_1);  // TODO: add error
                    //                   handling
                    err = op->func(&result, operand_2, operand_1);
                    if (err) {
                        stack_free(st);
                        string_free(token);
                        return err;
                    }

                    err = stack_push(st, &result);
                    if (err) {
                        log_error("Error push to stack");
                        stack_free(st);
                        string_free(token);
                        return err;
                    }
                } else {  // op is unary
                    err = stack_top(st, &p_for_stack);
                    if (err != EXIT_SUCCESS && err != STACK_IS_EMPTY) {
                        log_error("Error top from stack");
                        stack_free(st);
                        string_free(token);
                        return err;
                    }
                    if (err == STACK_IS_EMPTY) {
                        log_error(
                            "stack is empty, not enouth operands for "
                            "operators");
                        stack_free(st);
                        string_free(token);
                        return INVALID_OPERATIONS;
                    }
                    operand_1 = *(int *)p_for_stack->data;
                    err = stack_pop(st);
                    if (err) {
                        log_error("Error pop from stack");
                        stack_free(st);
                        string_free(token);
                        return err;
                    }
                    // result = op->func(operand_1);  // TODO: add error
                    // handling
                    err = op->func(&result, operand_1);
                    if (err) {
                        stack_free(st);
                        string_free(token);
                        return err;
                    }

                    err = stack_push(st, &result);
                    if (err) {
                        log_error("Error push to stack");
                        stack_free(st);
                        string_free(token);
                        return err;
                    }
                }
            }

            string_free(token);
            token = NULL;
            token = string_init();
            if (token == NULL) {
                log_error("failed to allocate memory for string");
                stack_free(st);
                string_free(token);
                return MEMORY_ALLOCATION_ERROR;
            }
        } else {
            err = string_add(&token, pe);
            if (err) {
                log_error("Error while adding to string");
                stack_free(st);
                string_free(token);
                return err;
            }
        }
    }

    if (stack_is_empty(st)) {
        *expression_result = 0;
        string_free(token);
        stack_free(st);
        return EXIT_SUCCESS;
    }
    err = stack_top(st, &p_for_stack);
    if (err) {  // stack is not empty
        log_error("failed top from stack");
        string_free(token);
        stack_free(st);
        return err;
    }
    result = *(int *)p_for_stack->data;
    err = stack_pop(st);
    if (err) {  // stack is not empty
        log_error("failed pop from stack");
        string_free(token);
        stack_free(st);
        return err;
    }

    if (!stack_is_empty(st)) {
        log_error(
            "evaluation ended, stack is not empty (%zu), invalid operators and "
            "operands combination, %s",
            st->size, postfix_exp);
        string_free(token);
        stack_free(st);
        return INVALID_OPERATIONS;
    }

    *expression_result = result;

    stack_free(st);
    string_free(token);

    return EXIT_SUCCESS;
}
