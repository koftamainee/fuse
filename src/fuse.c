#include "fuse.h"

#include <stdlib.h>

#include "../include/logger.h"

err_t convert_infix_to_postfix_notation(const String infix_expression,
                                        int (*priority_mapper)(char c),
                                        int (*characters_validator)(char c),
                                        String *postfix_expression) {
    if (infix_expression == NULL) {
        log_error("infix_expression is NULL");
        return DEREFERENCING_NULL_PTR;
    }

    if (postfix_expression == NULL) {
        log_error("postfix expression is NULL");
        return DEREFERENCING_NULL_PTR;
    }

    if (priority_mapper == NULL) {
        log_error("priority_mapper is NULL");
        return DEREFERENCING_NULL_PTR;
    }

    if (characters_validator == NULL) {
        log_error("characters_validator is NULL");
        return DEREFERENCING_NULL_PTR;
    }

    return EXIT_SUCCESS;
}

err_t calculate_postfix_expression(const String postfix_expression,
                                   uint8_t base_assign, String *result) {
    if (postfix_expression == NULL) {
        log_error("postfix_expression is NULL");
        return DEREFERENCING_NULL_PTR;
    }

    if (result == NULL) {
        log_error("result is NULL");
        return DEREFERENCING_NULL_PTR;
    }

    if (base_assign < 2 || base_assign > 36) {
        log_error("Invalid base_assign: %ud", base_assign);
        return INVALID_BASE;
    }

    return EXIT_SUCCESS;
}
