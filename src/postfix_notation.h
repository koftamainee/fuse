#ifndef POSTFIX_NOTATION_H_
#define POSTFIX_NOTATION_H_

#include "../include/cstring.h"
#include "../include/errors.h"
#include "../include/hash_table.h"
#include "../include/u_list.h"

typedef enum { unary, binary } operator_type;

typedef struct {
    operator_type type;
    int priority;
    int (*func)(int, ...);
} operator_t;

err_t infix_to_postfix(const String infix_exp, int (*is_operand)(int c),
                       int (*is_operator)(const char *op, u_list *operators_ul),
                       u_list *operators_ht,
                       int (*priority_mapper)(const String op),
                       String *postfix_exp);

// TODO: redo with trie
// err_t calculate_postfix_expression(const String postfix_exp,
//                                    int *expression_result,
//                                    hash_table *operators, hash_table
//                                    *operands);

#endif
