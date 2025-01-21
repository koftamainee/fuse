#ifndef FUSE_H_
#define FUSE_H_

#include "../include/cstring.h"
#include "../include/errors.h"
#include "../include/hash_table.h"
#include "cli.h"

typedef enum { unary, binary } operator_type;

typedef struct {
    operator_type type;
    int priority;
    err_t (*func)(int *, ...);
} operator_t;

err_t fuse_start(CLIOptions *cli_opts);

err_t create_ht_with_operators(hash_table *operators);

err_t create_ht_with_real_names(hash_table *names);
#endif
