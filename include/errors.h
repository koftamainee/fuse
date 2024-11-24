#ifndef ERRORS_H_
#define ERRORS_H_

#include <stdint.h>

typedef int32_t err_t;

#define DEREFERENCING_NULL_PTR (-1)
#define MEMORY_ALLOCATION_ERROR (-2)
#define WARNING_PASSED_STRING_TO_INIT_IS_NOT_NULL (-3)

#define UNKNOWN_CLI_ARGUMENT (1)
#define ERROR_READING_FROM_STDIN (2)
#define INTEGER_OVERFLOW (3)
#define UINT8_T_OVERFLOW (4)

#endif