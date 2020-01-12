#ifndef _TRACE_IF_H_
#define _TRACE_IF_H_

#ifdef __cplusplus
extern "C"
{
#endif


#define TRACE_DEBUG_PRINTF_ENABLED  (1)


//#include "generic_uart.hpp"
#ifdef TRACE_DEBUG_PRINTF_ENABLED
#define DEBUG_PRINTF(...) (Trace_debug_printf(__VA_ARGS__))
#else
#define DEBUG_PRINTF(...)
#endif


/** This function will printout a trace log over the standard debug interface */
void Trace_debug_printf(const char *, ...);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* _TRACE_IF_H_ */
