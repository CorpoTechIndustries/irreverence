#pragma once

#include <stdarg.h>

typedef enum {
	LOG_LEVEL_INFO,
	LOG_LEVEL_WARNING,
	LOG_LEVEL_ERROR,
	LOG_LEVEL_FATAL
} log_level_t;

typedef void(*log_callback_t)(log_level_t level, const char* fmt, va_list args);

void Log_Message(log_level_t level, const char* fmt, ...);
void Log_SetCallback(log_callback_t cb);

#define LOG_MESSAGE(__level, __fmt, ...) \
Log_Message(__level, __fmt __VA_OPT__(,) __VA_ARGS__)

#define LOG_INFO(fmt, ...) LOG_MESSAGE(LOG_LEVEL_INFO, fmt, __VA_ARGS__)
#define LOG_WARNING(fmt, ...) LOG_MESSAGE(LOG_LEVEL_WARNING, fmt, __VA_ARGS__)
#define LOG_ERROR(fmt, ...) LOG_MESSAGE(LOG_LEVEL_ERROR, fmt, __VA_ARGS__)
#define LOG_FATAL(fmt, ...) LOG_MESSAGE(LOG_LEVEL_FATAL, fmt, __VA_ARGS__)
