


/* System header */
#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	//#include "WProgram.h"
#endif

#include <cstdarg>
#include <cstring>
#include <string>
#include <mutex>
#include <memory>

#include "generic_uart.hpp"


#include "stm32f3xx.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

/* Own header */
#include "midware/trace/trace.h"

//std::mutex mutex_serial_if;
std::shared_ptr<drivers::GenericUART> m_p_uart = nullptr;

void set_serial_output(const std::shared_ptr<drivers::GenericUART> &p_uart)
{
	m_p_uart = p_uart;
}
void print_serial(const char character)
{
	//mutex_serial_if.lock();
	//Serial.println(character);
	//mutex_serial_if.unlock();
}

void print_serial(const char *buffer)
{
	if (m_p_uart != nullptr)
	{
		m_p_uart->write(reinterpret_cast<const uint8_t*>(buffer), strlen(buffer));
	}
	//mutex_serial_if.lock();

	printf(buffer);
#if 1
	  int i=0;
	  for(i=0 ; i<strlen(buffer) ; i++)
	  {
		  ITM_SendChar(buffer[i]);
	  }
	  //return len;
#else
	// For Arduino systems
	Serial.println(buffer);
#endif
	//mutex_serial_if.unlock();
}


void print_trace(const char * format_str, ...)
{
	char print_buffer[1024];
	va_list args;

	va_start(args, format_str);
	snprintf(print_buffer, 1024, format_str, args);
	va_end(args);

	print_serial(print_buffer);
}

void print_trace(const std::string &str)
{
	print_serial(str.c_str());
}

void exception_handler(const exception_type type, const std::string &str)
{
	if (EXC_FATAL == type)
	{
		print_trace(std::string("[FATAL] ") + str);

		/* Forever loop current thread to allow debugging stack trace */
		while(true)
		{
		}
	}
	else if (EXC_MALFUNC == type)
	{
		print_trace(std::string("[MALFUNC] ") + str);
	}
}

void print_heap()
{
	char buffer123[200] = { 0 };
	snprintf(buffer123, 200, "stack %u, current free heap: %d, minimum ever free heap: %d", uxTaskGetStackHighWaterMark(NULL), xPortGetFreeHeapSize(), xPortGetMinimumEverFreeHeapSize());
	DEBUG_PRINTF(buffer123);
}
