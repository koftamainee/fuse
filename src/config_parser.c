#include "config_parser.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/logger.h"
#include "fuse.h"

void free_for_u_list(void *a) {
    string_free(*(String *)a);
    free(a);
}

err_t parse_config_file(FILE *config_file, execution_options *options) {
    if (config_file == NULL) {
        log_fatal("passed ptr is NULL");
        return DEREFERENCING_NULL_PTR;
    }

    char line[BUFSIZ], *current = NULL, c = 0;
    String instruction = NULL, replacement = NULL, old_op = NULL;
    err_t err = 0;
    int in_comment = 0;
    int i = 0;
    size_t current_word_count = 0;
    operator_t *op = NULL, *to_replace_op = NULL;
    u_list *to_dispose = NULL;
    u_list_node *current_node = NULL;
    String current_c = NULL;

    log_info("starting parsing config file");

    log_error("before: %zu", options->operators->size);

    err = u_list_init(&to_dispose, sizeof(String *), free_for_u_list);
    if (err) {
        log_fatal("failed to init u_list");
        return err;
    }

    while (fgets(line, sizeof(line), config_file)) {
        current = line;
        instruction = string_init();
        if (instruction == NULL) {
            log_fatal("memory allocation error");
            u_list_free(to_dispose);
            return MEMORY_ALLOCATION_ERROR;
        }
        while (*current) {
            if (*current == '[') {
                in_comment = 1;
            } else if (*current == ']') {
                current++;
                in_comment = 0;
                if (*current == '\0') {
                    break;
                }
            } else if (*current == '#') {
                break;
            }
            if (!in_comment && *current != '\n') {
                err = string_add(&instruction, *current);
                if (err) {
                    log_fatal("failed add to string");
                    string_free(instruction);
                    u_list_free(to_dispose);
                    return err;
                }
            }
            current++;
        }

        if (string_len(instruction) == 0) {
            string_free(instruction);
            continue;
        }

        if (strncmp("op()", instruction, 4) == 0) {
            options->ex_t = prefix;
            log_trace("op() instruction found");
        } else if (strncmp("(op)", instruction, 4) == 0) {
            log_trace("(op) instruction found");
            options->ex_t = infix;
        } else if (strncmp("()op", instruction, 4) == 0) {
            log_trace("()op instruction found");
            options->ex_t = postfix;
        } else if (strncmp("left=", instruction, 5) == 0) {
            log_trace("left= instruction found");
            options->eq_t = left_eq;
        } else if (strncmp("right=", instruction, 5) == 0) {
            log_trace("right= instruction found");
            options->eq_t = right_eq;
        } else {
            old_op = string_init();
            if (old_op == NULL) {
                log_fatal("memory alloc failed");
                string_free(instruction);
                u_list_free(to_dispose);
                return MEMORY_ALLOCATION_ERROR;
            }
            for (i = 0; i < string_len(instruction); ++i) {
                c = instruction[i];
                if (c == ' ') {  // border principle
                    replacement = string_init();
                    if (replacement == NULL) {
                        log_fatal("failed to allocate memory");
                        string_free(instruction);
                        u_list_free(to_dispose);
                        string_free(old_op);
                        return MEMORY_ALLOCATION_ERROR;
                    }
                    err = string_grow(&replacement,
                                      string_len(instruction) - i - 1);
                    if (err) {
                        log_fatal("string growing error");
                        string_free(instruction);
                        string_free(old_op);
                        u_list_free(to_dispose);
                        return err;
                    }
                    memcpy(replacement, instruction + i + 1,
                           string_len(instruction) - i - 1);
                    __cstring_string_to_base(replacement)->length =
                        string_len(instruction) - i - 1;
                    break;

                } else {
                    err = string_add(&old_op, c);
                    if (err) {
                        log_fatal("failed add to string");
                        string_free(instruction);
                        string_free(replacement);
                        string_free(old_op);
                        u_list_free(to_dispose);
                        return err;
                    }
                }
            }

            err = hash_table_get(options->operators, &old_op, (void **)&op);
            if (err) {
                if (err == KEY_NOT_FOUND) {
                    log_fatal("invalid operator in config file: %s", old_op);
                    string_free(old_op);
                    string_free(replacement);
                    string_free(instruction);
                    u_list_free(to_dispose);
                    return INVALID_SYNTAX;
                } else {
                    log_fatal("failed to get from hash table: %zu", err);
                }
                string_free(old_op);
                string_free(replacement);
                u_list_free(to_dispose);
                string_free(instruction);
                return err;
            }
            to_replace_op = (operator_t *)malloc(sizeof(operator_t));
            if (to_replace_op == NULL) {
                log_fatal("failed to allocate memory");
                string_free(old_op);
                string_free(replacement);
                string_free(instruction);
                u_list_free(to_dispose);
                return MEMORY_ALLOCATION_ERROR;
            }
            to_replace_op->func = op->func;
            to_replace_op->type = op->type;
            to_replace_op->priority = op->priority;

            err = u_list_insert(to_dispose, 0, &old_op);
            if (err) {
                log_fatal("failed push back to list");
                free(to_replace_op);
                string_free(old_op);
                string_free(replacement);
                string_free(instruction);
                u_list_free(to_dispose);
                return err;
            }

            string_add(&old_op, '\0');
            string_add(&replacement, '\0');
            __cstring_string_to_base(replacement)->length--;
            __cstring_string_to_base(old_op)->length--;

            err =
                hash_table_set(options->operators, &replacement, to_replace_op);
            if (err) {
                log_fatal("failed set to hash table");
                free(to_replace_op);
                string_free(old_op);
                string_free(replacement);
                string_free(instruction);
                u_list_free(to_dispose);
                return err;
            }

            log_trace("replaced \"%s\" to \"%s\"", old_op, replacement);

            // string_free(old_op);
            old_op = NULL;
        }

        string_free(instruction);
        free(to_replace_op);
        instruction = NULL;
    }

    current_node = to_dispose->first;
    log_error("after: %zu", options->operators->size);
    log_error("to delete: %zu", to_dispose->size);

    while (current_node) {
        err = hash_table_dispose(options->operators, current_node->data);
        if (err != EXIT_SUCCESS && err != KEY_NOT_FOUND) {
            log_fatal("failed dispose from hash table: %d", err);
            u_list_free(to_dispose);
            return err;
        }
        current_node = current_node->next;
    }

    log_error("done: %zu", options->operators->size);

    u_list_free(to_dispose);

    return EXIT_SUCCESS;
}
