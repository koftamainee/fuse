#include "logger.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX_LOGGERS 16

typedef struct {
    FILE *fp;
    log_level level;
} Logger;  // loggers instance

static struct {
    log_level level;
    Logger loggers[MAX_LOGGERS];
    size_t loggers_count;
} L = {LOG_INFO, {{NULL, LOG_INFO}}, 0};

static const char *level_string[] = {"TRACE", "DEBUG", "INFO",
                                     "WARN",  "ERROR", "FATAL"};

static void log_to_stream(FILE *stream, log_level level, const char *file,
                          int line, const char *fmt, va_list ap) {
    char time_buf[16];
    time_t t = time(NULL);
    strftime(time_buf, sizeof(time_buf), "%H:%M:%S",
             localtime(&t));  // Format time
    fprintf(stream, "%s %-5s %s:%d: ", time_buf, level_string[level], file,
            line);              // Prints time and log state
    vfprintf(stream, fmt, ap);  // Print user-defined data
    fprintf(stream, "\n");
    fflush(stream);
}

void log_set_level(log_level level) { L.level = level; }

err_t log_add_fp(FILE *fp, log_level level) {
    if (fp == NULL) {
        fprintf(stderr,
                "Error: Passed file pointer is NULL. Aborting adding new "
                "logger.\n");
        return DEREFERENCING_NULL_PTR;
    }

    if (L.loggers_count >= MAX_LOGGERS) {
        fprintf(
            stderr,
            "Error: Reached max logger count. Aborting adding new logger.\n");
        return ERROR_MAX_LOGGER_COUNT_REACHED;
    }

    L.loggers[L.loggers_count++] = (Logger){fp, level};
    return EXIT_SUCCESS;
}

void log_log(log_level level, const char *file, int line, const char *fmt,
             ...) {
    if (fmt == NULL) {
        fprintf(stderr, "Warning: fmt passed to log is NULL.\n");
        return;
    }

    if (level < L.level) {
        return;  // Skip logs below the global level
    }

    va_list ap;
    va_start(ap, fmt);

    // Log to all logger instances
    for (size_t i = 0; i < L.loggers_count; ++i) {
        if (level >= L.loggers[i].level) {
            va_list ap_cpy;
            va_copy(ap_cpy, ap);
            log_to_stream(L.loggers[i].fp, level, file, line, fmt, ap_cpy);
            va_end(ap_cpy);  // Clean up copied va_list
        }
    }

    va_end(ap);
}
