#ifndef PREFIX_NOTATION_H
#define PREFIX_NOTATION_H

#include "../include/cstring.h"
#include "../include/errors.h"
#include "../include/hash_table.h"
#include "../include/u_list.h"
#include "cli.h"
#include "fuse.h"

err_t prefix_to_postfix(const String prefix_exp, int (*is_operand)(int c),
                        hash_table *operators_ht, String *postfix_exp);

#endif  // !PREFIX_NOTATION_H
