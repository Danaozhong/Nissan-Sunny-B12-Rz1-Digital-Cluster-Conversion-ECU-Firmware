#ifndef _TRACE_IF_H_
#define _TRACE_IF_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

#ifdef USE_TRACE
#define TRACE_DEBUG_PRINTF_ENABLED  (1)
#define TRACE_CONTEXT_LOGGING_ENABLED (1)
#endif


#ifdef TRACE_DEBUG_PRINTF_ENABLED
#define DEBUG_PRINTF(...) (Trace_debug_printf(__VA_ARGS__))
#else
#define DEBUG_PRINTF(...)
#endif

#ifdef TRACE_CONTEXT_LOGGING_ENABLED
#define TRACE_DECLARE_CONTEXT(context)  (Trace_declare_context(context))
#define TRACE_SET_LOG_LEVEL(context, log_level)  (Trace_set_log_level(context, log_level))
#define TRACE_LOG(context, log_level, ...)  (Trace_log(context, log_level, __VA_ARGS__))
#else
#define TRACE_DECLARE_CONTEXT(context)
#define TRACE_SET_LOG_LEVEL(context, log_level)
#define TRACE_LOG(context, log_level, ...)
#endif


typedef enum
{
    LOGLEVEL_DEBUG,
    LOGLEVEL_INFO,
    LOGLEVEL_WARNING,
    LOGLEVEL_ERROR,
    LOGLEVEL_FATAL
} TraceLogLevel;


/** This function will printout a trace log over the standard debug interface */
void Trace_debug_printf(const char *, ...);

int32_t Trace_declare_context(const char* context);

/** Sets the log level for a specific context. */
int32_t Trace_set_log_level(const char* context, TraceLogLevel log_level);

/**
 *
 */
void Trace_log(const char* context, TraceLogLevel log_level, const char *, ...);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* _TRACE_IF_H_ */
