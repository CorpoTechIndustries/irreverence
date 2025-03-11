#include <engine/log.h>
#include <platform/io.h>

static const char* prefixes[LOG_LEVEL_FATAL + 1] = {
	"INFO",
	"WARNING",
	"ERROR",
	"FATAL"
};

static void default_cb(log_level_t level, const char* fmt, va_list args)
{
	Sys_Printf("[%s]: ", prefixes[level]);
	Sys_VPrintf(fmt, args);
	Sys_Print("\n");
}

static log_callback_t s_pLogCallback = default_cb;

void Log_Message(log_level_t level, const char* fmt, ...)
{
	va_list args;

	va_start(args, fmt);

	s_pLogCallback(level, fmt, args);

	va_end(args);
}
