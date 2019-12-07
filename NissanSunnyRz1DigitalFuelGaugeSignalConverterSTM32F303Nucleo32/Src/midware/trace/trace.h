#ifndef _TRACE_H_
#define _TRACE_H_


#include "generic_uart.hpp"
#define DEBUG_PRINTF(...) (print_trace(__VA_ARGS__))

#define MALFUNC(a) (exception_handler(EXC_MALFUNC, a)) //(print_trace(std::string("[MALFUNC] ") + a))
#define FATAL(a) (exception_handler(EXC_FATAL, a))  // (print_trace(std::string("[FATAL] ") + a))

void print_trace(const char *, ...);


void set_serial_output(drivers::GenericUART* p_uart);



enum exception_type
{
	EXC_FATAL,
	EXC_MALFUNC,
	EXC_INFO

};

#endif /* _TRACE_H_ */
