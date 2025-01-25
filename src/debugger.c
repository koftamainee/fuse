#include "debugger.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/custom_math.h"
#include "../include/logger.h"
#include "../include/types.h"
#include "fuse.h"

err_t start_debugger(hash_table *variables) {
    if (variables == NULL) {
        log_fatal("passed ptr is NULL");
        return DEREFERENCING_NULL_PTR;
    }

    char c;

    char line[BUFSIZ];
    char *instruction = NULL;
    err_t err = 0;

    fflush(stdin);

    while ((c = getchar()) && c != '\n' && c != EOF);

    while (1) {
        printf("(fdb) ");
        scanf("%s", line);
        for (instruction = line; isspace(*instruction); ++instruction);
        if (strcmp(instruction, "p") == 0) {
            err = debugger_print_variable(variables);
            if (err) {
                return err;
            }
        } else if (strcmp(instruction, "v") == 0) {
            err = debugger_show_all_variables(variables);
            if (err) {
                return err;
            }
        } else if (strcmp(instruction, "c") == 0) {
            err = debugger_change_variable(variables);
            if (err) {
                return err;
            }
        } else if (strcmp(instruction, "i") == 0) {
            err = debugger_initialize_variable(variables);
            if (err) {
                return err;
            }
        } else if (strcmp(instruction, "d") == 0) {
            err = debugger_deinitialize_variable(variables);
            if (err) {
                return err;
            }
        } else if (strcmp(instruction, "q") == 0) {
            return EXIT_SUCCESS;
        } else if (strcmp(instruction, "k") == 0) {
            return KILL_PROGRAM;
        } else if (strcmp(instruction, "h") == 0) {
            debugger_show_help();
        } else if (instruction[0] != '\0') {
            printf("Invalid command. Enter 'h' to show all commands\n");
        }
    }

    return EXIT_SUCCESS;
}

err_t debugger_print_variable(hash_table *variables) {
    if (variables == NULL) {
        log_fatal("passed ptr is NULL");
        return DEREFERENCING_NULL_PTR;
    }

    String name = NULL;
    char buf[BUFSIZ];
    err_t err = 0;
    int *for_ht = NULL, i = 0, j = 0;
    char *result;
    unsigned char *uc_ptr = NULL;
    size_t binary_len;

    printf("Enter variable name: ");

    scanf("%s", buf);
    name = string_from(buf);
    if (name == NULL) {
        log_fatal("memory allocation error");
        return MEMORY_ALLOCATION_ERROR;
    }
    err = string_add(&name, '\0');
    if (err) {
        log_fatal("failed add to string");
        string_free(name);
        return err;
    }

    __cstring_string_to_base(name)->length--;

    err = hash_table_get(variables, &name, (void **)&for_ht);
    if (err != EXIT_SUCCESS && err != KEY_NOT_FOUND) {
        log_fatal("failed to get from hash table");
        string_free(name);
        return err;
    }
    if (err == KEY_NOT_FOUND) {
        printf("Variable \"%s\" is not initialized\n", name);
        string_free(name);
        return EXIT_SUCCESS;
    }

    printf("%s: ", name);
    string_free(name);

    err = citoa(*for_ht, 16, &result);
    if (err) {
        log_fatal("failed to convert int to string");
        return err;
    }
    printf("0x%s | ", result);
    free(result);

    uc_ptr = (unsigned char *)for_ht;
    for (i = 0; i < sizeof(int); ++i) {
        err = citoa(*uc_ptr, 2, &result);
        if (err) {
            free(result);
            result = NULL;
            return err;
        }
        binary_len = strlen(result);
        for (j = binary_len; j < 8; ++j) {
            printf("0");
        }
        printf("%s ", result);
        free(result);
        result = NULL;
        uc_ptr++;
    }
    printf("\n");

    return EXIT_SUCCESS;
}

err_t debugger_show_all_variables(hash_table *variables) {
    if (variables == NULL) {
        log_fatal("passed ptr is NULL");
        return DEREFERENCING_NULL_PTR;
    }

    int i = 0;
    u_list *bucket = NULL;
    u_list_node *current = NULL;
    String variable_name = NULL;

    for (i = 0; i < variables->capacity; ++i) {
        bucket = variables->buckets[i];
        current = bucket->first;
        while (current != NULL) {
            variable_name =
                *(String *)((hash_table_bucket *)current->data)->key;
            string_print(variable_name);
            printf(" ");
            current = current->next;
        }
    }
    printf("\n");

    return EXIT_SUCCESS;
}

err_t debugger_change_variable(hash_table *variables) {
    if (variables == NULL) {
        log_fatal("passed ptr is NULL");
        return DEREFERENCING_NULL_PTR;
    }

    String name = NULL;
    char buf[BUFSIZ];
    err_t err = 0;
    int result = 0;

    printf("Enter variable name: ");

    scanf("%s", buf);
    name = string_from(buf);
    if (name == NULL) {
        log_fatal("memory allocation error");
        return MEMORY_ALLOCATION_ERROR;
    }

    printf("Enter new value in hex: ");
    scanf("%s", buf);
    err = catoi(buf, 16, &result);
    if (err) {
        printf("Failed to parse int from string\n");
        string_free(name);
        return EXIT_SUCCESS;
    }

    err = hash_table_dispose(variables, &name);
    if (err != EXIT_SUCCESS && err != KEY_NOT_FOUND) {
        log_fatal("Failed to dispose from hash table");
        string_free(name);
        return err;
    }
    if (err == KEY_NOT_FOUND) {
        printf("No such variable declared.\n");
    }

    err = hash_table_set(variables, &name, &result);
    if (err) {
        log_fatal("failed to set to hash table");
        string_free(name);
        return err;
    }

    return EXIT_SUCCESS;
}

int is_valid_rome(char c) {
    return c == 'I' || c == 'V' || c == 'X' || c == 'L' || c == 'C' ||
           c == 'D' || c == 'M';
}

int rometoi(char *str, int *ans) {
    int len = strlen(str);
    int i = 0, j = 0;
    char letter_1, letter_2;
    int temp_int = 0;
    int found = 0;

    struct roman_item {
        int val;
        int len;
        char *letter;
    };

    struct roman_item roman_data[] = {
        {1000, 1, "M"}, {900, 2, "CM"}, {500, 1, "D"}, {400, 2, "CD"},
        {100, 1, "C"},  {90, 2, "XC"},  {50, 1, "L"},  {40, 2, "XL"},
        {10, 1, "X"},   {9, 2, "IX"},   {5, 1, "V"},   {4, 2, "IV"},
        {1, 1, "I"}};

    for (i = 0; i < len; i++) {
        if (!is_valid_rome(str[i])) {
            return INVALID_CHAR;
        }
    }

    i = 0;
    while (i < len) {
        letter_1 = str[i];
        letter_2 = (i + 1 < len) ? str[i + 1] : '\0';
        temp_int = 0;

        found = 0;
        for (j = 0; j < 13; j++) {
            if (roman_data[j].len == 2 && roman_data[j].letter[0] == letter_1 &&
                roman_data[j].letter[1] == letter_2) {
                temp_int = roman_data[j].val;
                i++;
                found = 1;
                break;
            }
            if (roman_data[j].len == 1 && roman_data[j].letter[0] == letter_1) {
                temp_int = roman_data[j].val;
                found = 1;
                break;
            }
        }

        if (!found) {
            return INVALID_CHAR;
        }

        (*ans) += temp_int;
        i++;
    }

    return EXIT_SUCCESS;
}
err_t debugger_initialize_variable(hash_table *variables) {
    if (variables == NULL) {
        log_fatal("passed ptr is NULL");
        return DEREFERENCING_NULL_PTR;
    }

    String name = NULL;
    char buf[BUFSIZ];
    err_t err = 0;
    int *for_ht = NULL, user_ans = 0;
    char c, prev = 0;
    int result = 0;
    int count = 0, *fib_arr = NULL, i = 0, temp_int = 0;
    size_t len = 0;

    printf("Enter variable name: ");

    scanf("%s", buf);
    name = string_from(buf);
    if (name == NULL) {
        log_fatal("memory allocation error");
        return MEMORY_ALLOCATION_ERROR;
    }

    err = hash_table_get(variables, &name, (void *)&for_ht);
    if (err != EXIT_SUCCESS && err != KEY_NOT_FOUND) {
        log_fatal("failed to get from hash table");
        string_free(name);
        return err;
    }
    if (err == EXIT_SUCCESS) {
        printf("Variable is already declared.\n");
        string_free(name);
        return EXIT_SUCCESS;
    }

    for (i = 0; i < string_len(name); ++i) {
        if (!is_operand(name[i])) {
            printf("Invalid variable name.\n");
            string_free(name);
            return EXIT_SUCCESS;
        }
    }
    while (user_ans != 1 && user_ans != 2) {
        printf("Enter value by:\n");
        printf("1. Zekendorf counting system\n2. Roman numbers\n");
        printf("Choose: ");
        scanf("%d", &user_ans);
        if (user_ans != 1 && user_ans != 2) {
            while ((c = getchar()) && c != '\n' && c != EOF);
            printf("Error. Try again\n");
        }
        printf("Enter value: ");
        scanf("%s", buf);
        switch (user_ans) {
            case 1:
                len = strlen(buf);
                fib_arr = (int *)malloc(len * sizeof(int));
                err = generate_fibonacci_row_upto_n(len, fib_arr, &count);
                if (err) {
                    log_fatal("failed to generate fib");
                    string_free(name);
                    return err;
                }
                temp_int = 0;
                for (i = 0; i < len; i++) {
                    if ((buf[i] != '1' && buf[i] != '0') ||
                        (buf[i] == '1' && prev == '1')) {
                        printf("Invalid Fibbonacci number\n");
                        free(fib_arr);
                        string_free(name);
                        return EXIT_SUCCESS;
                    }
                    if (buf[i] == '1') {
                        temp_int += fib_arr[len - i - 1];
                    }
                    prev = buf[i];
                }
                free(fib_arr);
                result = temp_int;
                break;

            case 2:
                err = rometoi(buf, &result);
                if (err) {
                    log_fatal("failed to convert rome to int");
                    string_free(name);
                    return err;
                }
                break;
        }
    }

    err = hash_table_set(variables, &name, &result);
    if (err) {
        log_fatal("failed to set to hash table");
        string_free(name);
        return err;
    }

    return EXIT_SUCCESS;
}

err_t debugger_deinitialize_variable(hash_table *variables) {
    if (variables == NULL) {
        log_fatal("passed ptr is NULL");
        return DEREFERENCING_NULL_PTR;
    }

    String name = NULL;
    char buf[BUFSIZ];
    err_t err = 0;

    printf("Enter variable name: ");

    scanf("%s", buf);
    name = string_from(buf);
    if (name == NULL) {
        log_fatal("memory allocation error");
        return MEMORY_ALLOCATION_ERROR;
    }

    err = hash_table_dispose(variables, &name);
    if (err != EXIT_SUCCESS && err != KEY_NOT_FOUND) {
        log_fatal("failed to dispose from hash table");
        string_free(name);
        return err;
    }
    if (err == KEY_NOT_FOUND) {
        printf("No such variable declared.\n");
    }

    string_free(name);
    return EXIT_SUCCESS;
}

void debugger_show_help() {
    printf(
        "p - print variable\n"
        "v - show all initialized variables\n"
        "c - change variable value\n"
        "i - initialize new variable\n"
        "d - deinitiaize variable\n"
        "q - continue execution\n"
        "k - kill program\n"
        "h - show help screen\n");
}
