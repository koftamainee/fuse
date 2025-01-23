#include "config_parser.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/logger.h"
#include "fuse.h"

int real_names_string_comparer(const void *a, const void *b) {
    return string_cmp(*(const String *)a, *(const String *)b);
}
void real_names_bucket_destructor(void *a) {
    hash_table_bucket *b = a;
    string_free(*(String *)b->key);
    string_free(*(String *)b->value);
    free(b->key);
    free(b->value);
    free(b);
}

err_t parse_config_file(FILE *config_file, execution_options *options,
                        String *equation_operator_placeholder,
                        String *input_operator_placeholder,
                        String *output_operator_placeholder) {
    if (config_file == NULL || options == NULL ||
        equation_operator_placeholder == NULL ||
        input_operator_placeholder == NULL ||
        output_operator_placeholder == NULL) {
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
    hash_table *real_names = NULL;
    String *for_real_name = NULL, new = NULL, old = NULL;
    String equation_operator = NULL, output_operator = NULL,
           input_operator = NULL;

    log_info("starting parsing config file");

    err = hash_table_init(&real_names, real_names_string_comparer, djb2_hash,
                          sizeof(String *), sizeof(String *),
                          real_names_bucket_destructor);
    if (err) {
        log_fatal("failed to init hash table");
        return err;
    }

    equation_operator = string_from("=");
    if (equation_operator == NULL) {
        log_fatal("failed to allocate memory");
        hash_table_free(real_names);
        return MEMORY_ALLOCATION_ERROR;
    }
    input_operator = string_from("input");
    if (equation_operator == NULL) {
        log_fatal("failed to allocate memory");
        string_free(equation_operator);
        hash_table_free(real_names);
        return MEMORY_ALLOCATION_ERROR;
    }
    output_operator = string_from("output");
    if (equation_operator == NULL) {
        log_fatal("failed to allocate memory");
        string_free(equation_operator);
        string_free(input_operator);
        hash_table_free(real_names);
        return MEMORY_ALLOCATION_ERROR;
    }

    err = create_ht_with_real_names(real_names);
    if (err) {
        string_free(equation_operator);
        string_free(input_operator);
        string_free(output_operator);
        hash_table_free(real_names);
        return err;
    }

    while (fgets(line, sizeof(line), config_file)) {
        current = line;
        instruction = string_init();
        if (instruction == NULL) {
            log_fatal("memory allocation error");
            hash_table_free(real_names);
            string_free(equation_operator);
            string_free(input_operator);
            string_free(output_operator);
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
                    hash_table_free(real_names);
                    string_free(equation_operator);
                    string_free(input_operator);
                    string_free(output_operator);
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
                hash_table_free(real_names);
                string_free(equation_operator);
                string_free(input_operator);
                string_free(output_operator);
                return MEMORY_ALLOCATION_ERROR;
            }
            for (i = 0; i < string_len(instruction); ++i) {
                c = instruction[i];
                if (c == ' ' && string_len(old_op) > 0) {
                    replacement = string_init();
                    if (replacement == NULL) {
                        log_fatal("failed to allocate memory");
                        string_free(instruction);
                        string_free(old_op);
                        string_free(equation_operator);
                        hash_table_free(real_names);
                        string_free(input_operator);
                        string_free(output_operator);
                        return MEMORY_ALLOCATION_ERROR;
                    }
                    for (; instruction[i] == ' ' || instruction[i] == '\t';
                         ++i);
                    i--;
                    err = string_grow(&replacement,
                                      string_len(instruction) - i - 1);
                    if (err) {
                        log_fatal("string growing error");
                        string_free(instruction);
                        string_free(old_op);
                        hash_table_free(real_names);
                        string_free(equation_operator);
                        string_free(input_operator);
                        string_free(output_operator);
                        return err;
                    }
                    memcpy(replacement, instruction + i + 1,
                           string_len(instruction) - i - 1);
                    __cstring_string_to_base(replacement)->length =
                        string_len(instruction) - i - 1;
                    break;

                } else {
                    if (c != ' ') {
                        err = string_add(&old_op, c);
                        if (err) {
                            string_free(input_operator);
                            string_free(output_operator);
                            log_fatal("failed add to string");
                            string_free(instruction);
                            string_free(replacement);
                            string_free(old_op);
                            hash_table_free(real_names);
                            string_free(equation_operator);
                            string_free(input_operator);
                            string_free(output_operator);
                            return err;
                        }
                    }
                }
            }

            string_add(&old_op, '\0');
            string_add(&replacement, '\0');
            __cstring_string_to_base(replacement)->length--;
            __cstring_string_to_base(old_op)->length--;

            if (string_len(replacement) == 0) {
                log_fatal(
                    "Invalid syntax in config file occured. Synonim for \"%s\" "
                    "doesn't specified",
                    old_op);
                string_free(old_op);
                string_free(replacement);
                string_free(instruction);
                hash_table_free(real_names);
                string_free(equation_operator);
                string_free(input_operator);
                string_free(output_operator);
                return INVALID_SYNTAX;
            }

            for (i = 0; i < string_len(replacement); ++i) {
                if (isspace(replacement[i]) && replacement[i] != '(' &&
                    replacement[i] != ')' && replacement[i] != ',') {
                    log_fatal("Invalid syntax occured. INvalid synonim: %s",
                              replacement);
                    string_free(old_op);
                    string_free(replacement);
                    string_free(instruction);
                    hash_table_free(real_names);
                    string_free(equation_operator);
                    string_free(input_operator);
                    string_free(output_operator);
                    return INVALID_SYNTAX;
                }
            }

            err = hash_table_get(real_names, &old_op, (void **)&for_real_name);
            if (err) {
                log_error(
                    "Invalid operator name: "
                    "\"%s\". \"%s\"",
                    old_op, replacement);
                string_free(old_op);
                string_free(replacement);
                string_free(instruction);
                hash_table_free(real_names);
                string_free(equation_operator);
                string_free(input_operator);
                string_free(output_operator);
                return err;
            }

            err =
                hash_table_get(options->operators, for_real_name, (void **)&op);
            if (err) {
                if (err == KEY_NOT_FOUND) {
                    log_fatal("invalid operator in config file: \"%s\"",
                              *for_real_name);
                    string_free(old_op);
                    string_free(replacement);
                    string_free(instruction);
                    hash_table_free(real_names);
                    string_free(equation_operator);
                    string_free(input_operator);
                    string_free(output_operator);
                    return INVALID_SYNTAX;
                } else {
                    log_fatal("failed to get from hash table: %zu", err);
                }
                string_free(old_op);
                string_free(replacement);
                string_free(instruction);
                hash_table_free(real_names);
                string_free(equation_operator);
                string_free(input_operator);
                string_free(output_operator);
                return err;
            }

            log_trace("replaced \"%s\" to \"%s\"", *for_real_name, replacement);

            err = hash_table_dispose(options->operators, for_real_name);
            if (err) {
                log_error("failed to dispose from hash table, %d, %s %s", err,
                          old_op, *for_real_name);
                string_free(old_op);
                string_free(replacement);
                string_free(instruction);
                hash_table_free(real_names);
                string_free(equation_operator);
                string_free(input_operator);
                string_free(output_operator);
                return err;
            }

            to_replace_op = (operator_t *)malloc(sizeof(operator_t));
            if (to_replace_op == NULL) {
                log_fatal("failed to allocate memory");
                string_free(old_op);
                string_free(replacement);
                string_free(instruction);
                hash_table_free(real_names);
                string_free(equation_operator);
                string_free(input_operator);
                string_free(output_operator);
                return MEMORY_ALLOCATION_ERROR;
            }
            to_replace_op->func = op->func;
            to_replace_op->type = op->type;
            to_replace_op->priority = op->priority;

            if (string_cmp_c(old_op, "=") == 0) {
                err = string_cpy(&equation_operator, &replacement);
                if (err) {
                    log_fatal("faileed to cpy string");
                    string_free(old_op);
                    string_free(replacement);
                    string_free(instruction);
                    hash_table_free(real_names);
                    string_free(equation_operator);
                    string_free(input_operator);
                    string_free(output_operator);
                    return err;
                }
            }
            if (string_cmp_c(old_op, "input") == 0) {
                err = string_cpy(&input_operator, &replacement);
                if (err) {
                    log_fatal("faileed to cpy string");
                    string_free(old_op);
                    string_free(replacement);
                    string_free(instruction);
                    hash_table_free(real_names);
                    string_free(equation_operator);
                    string_free(input_operator);
                    string_free(output_operator);
                    return err;
                }
            }
            if (string_cmp_c(old_op, "output") == 0) {
                err = string_cpy(&output_operator, &replacement);
                if (err) {
                    log_fatal("faileed to cpy string");
                    string_free(old_op);
                    string_free(replacement);
                    string_free(instruction);
                    hash_table_free(real_names);
                    string_free(equation_operator);
                    string_free(input_operator);
                    string_free(output_operator);
                    return err;
                }
            }

            old = string_from(old_op);
            new = string_from(replacement);

            err =
                hash_table_set(options->operators, &replacement, to_replace_op);
            if (err) {
                log_fatal("failed set to hash table");
                free(to_replace_op);
                string_free(old_op);
                string_free(replacement);
                string_free(instruction);
                hash_table_free(real_names);
                string_free(equation_operator);
                string_free(input_operator);
                string_free(output_operator);
                return err;
            }

            err = hash_table_dispose(real_names, &old);
            if (err) {
                log_fatal("failed to despose from hash table: %d", err);
                free(to_replace_op);
                string_free(old_op);
                string_free(replacement);
                string_free(instruction);
                hash_table_free(real_names);
                string_free(equation_operator);
                string_free(input_operator);
                string_free(output_operator);
                return err;
            }

            err = hash_table_set(real_names, &old, &new);
            if (err) {
                log_fatal("failed to set to hash table: %d", err);
                free(to_replace_op);
                string_free(old_op);
                string_free(replacement);
                string_free(instruction);
                hash_table_free(real_names);
                string_free(equation_operator);
                string_free(input_operator);
                string_free(output_operator);
                return err;
            }

            string_free(old_op);
            old_op = NULL;
        }

        string_free(instruction);
        free(to_replace_op);
        instruction = NULL;
    }

    *equation_operator_placeholder = equation_operator;
    *output_operator_placeholder = output_operator;
    *input_operator_placeholder = input_operator;
    log_info("Config file parsing finished");

    hash_table_free(real_names);

    return EXIT_SUCCESS;
}
