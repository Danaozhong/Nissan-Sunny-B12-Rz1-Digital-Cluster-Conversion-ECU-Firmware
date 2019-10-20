#ifndef _TRACE_H_
#define _TRACE_H_

#include <string>
#include <sstream>
#include <memory>
#include "generic_uart.hpp"
#define DEBUG_PRINTF(...) (print_trace(__VA_ARGS__))

#define MALFUNC(a) (exception_handler(EXC_MALFUNC, a)) //(print_trace(std::string("[MALFUNC] ") + a))
#define FATAL(a) (exception_handler(EXC_FATAL, a))  // (print_trace(std::string("[FATAL] ") + a))



void print_serial(const char character);

void print_serial(const char *buffer);

void print_trace(const char *, ...);


void set_serial_output(const std::shared_ptr<drivers::GenericUART> &p_uart);

/** C++ overload */
void print_trace(const std::string &str);


enum exception_type
{
	EXC_FATAL,
	EXC_MALFUNC,
	EXC_INFO

};

namespace std_ex
{
    template < typename T > std::string to_string( const T& n )
    {
        std::ostringstream stm ;
        stm << n ;
        return stm.str();
    }
}

void exception_handler(const exception_type type, const std::string &str);

#endif /* _TRACE_H_ */
