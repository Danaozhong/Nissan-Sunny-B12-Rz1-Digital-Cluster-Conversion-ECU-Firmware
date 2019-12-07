


/* System header */
#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	//#include "WProgram.h"
#endif

#include <cstdarg>
#include <cstring>
#include <mutex>

#include "generic_uart.hpp"


#include "stm32f3xx.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

/* Own header */
#include "trace.h"

#define TRACE_PRINTF_BUFFER_SIZE   (128u)

drivers::GenericUART* m_p_uart = nullptr;

void set_serial_output(drivers::GenericUART* p_uart)
{
	m_p_uart = p_uart;
}

void print_serial(const char *buffer)
{
	if (m_p_uart != nullptr)
	{
		m_p_uart->write(reinterpret_cast<const uint8_t*>(buffer), strlen(buffer));
	}

#if 0
	  int i=0;
	  for(i=0 ; i<strlen(buffer) ; i++)
	  {
		  ITM_SendChar(buffer[i]);
	  }
	  //return len;
#elif defined TEST1234
	// For Arduino systems
	Serial.println(buffer);
#endif
	//mutex_serial_if.unlock();
}


void print_trace(const char * format_str, ...)
{
	char print_buffer[TRACE_PRINTF_BUFFER_SIZE];
	va_list args;

	va_start(args, format_str);
	vsnprintf(print_buffer, TRACE_PRINTF_BUFFER_SIZE, format_str, args);
	va_end(args);

	print_serial(print_buffer);
}



void print_heap()
{
	char buffer123[TRACE_PRINTF_BUFFER_SIZE] = { 0 };
	snprintf(buffer123, TRACE_PRINTF_BUFFER_SIZE, "stack %u, current free heap: %d, minimum ever free heap: %d", uxTaskGetStackHighWaterMark(NULL), xPortGetFreeHeapSize(), xPortGetMinimumEverFreeHeapSize());
	DEBUG_PRINTF(buffer123);
}
