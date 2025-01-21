#include "fuse.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include "../include/logger.h"
#include "config_parser.h"

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

    if (cli_opts->input_file == NULL) {
        log_fatal("no input file specified");
        return NOT_ENOUTH_ARGUMENTS;
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

    err = parse_config_file(cli_opts->config_file, &exec_opts);
    if (err) {
        hash_table_free(exec_opts.operators);
        return err;
    }

    hash_table_free(exec_opts.operators);

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
        {"add", binary, 1, fuse_add},   {"sub", binary, 1, fuse_sub},
        {"mult", binary, 2, fuse_mult}, {"div", binary, 2, fuse_div},
        {"rem", binary, 2, fuse_rem},   {"pow", binary, 3, fuse_pow},
        {"and", binary, 0, fuse_and},   {"or", binary, 0, fuse_or},
        {"not", unary, 4, fuse_not},    {"xor", binary, 0, fuse_xor},
        {"input", unary, -1, NULL},     {"output", unary, -1, NULL},
        {"=", binary, -1, NULL}};

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
