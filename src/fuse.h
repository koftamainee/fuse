#ifndef FUSE_H_
#define FUSE_H_

#include "../include/cstring.h"
#include "../include/errors.h"

// math section
err_t convert_infix_to_postfix_notation(const String infix_expression,
                                        int (*priority_mapper)(char c),
                                        int (*characters_validator)(char c),
                                        String *postfix_expression);

err_t calculate_postfix_expression(const String postfix_expression,
                                   uint8_t base_assign, String *result);

#endif
