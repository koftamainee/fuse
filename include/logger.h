#ifndef LOGGER_H
#define LOGGER_H

#include <stdarg.h>
#include <stdio.h>
#include <time.h>

#include "errors.h"

typedef enum {
    LOG_IO,
    LOG_TRACE,
    LOG_DEBUG,
    LOG_INFO,
    LOG_WARN,
    LOG_ERROR,
    LOG_FATAL
} log_level;

#define log_io(...) log_log(LOG_IO, __FILE__, __LINE__, __VA_ARGS__)
#define log_trace(...) log_log(LOG_TRACE, __FILE__, __LINE__, __VA_ARGS__)
#define log_debug(...) log_log(LOG_DEBUG, __FILE__, __LINE__, __VA_ARGS__)
#define log_info(...) log_log(LOG_INFO, __FILE__, __LINE__, __VA_ARGS__)
#define log_warn(...) log_log(LOG_WARN, __FILE__, __LINE__, __VA_ARGS__)
#define log_error(...) log_log(LOG_ERROR, __FILE__, __LINE__, __VA_ARGS__)
#define log_fatal(...) log_log(LOG_FATAL, __FILE__, __LINE__, __VA_ARGS__)

void log_set_user_interaction(int enable);
void log_set_level(log_level level);
err_t log_add_fp(FILE *fp, log_level level);

void log_log(log_level level, const char *file, int line, const char *fmt, ...);
void vlog_log(log_level level, const char *file, int line, const char *fmt,
              va_list ap);

err_t logger_start();

void logger_stop();

#endif
