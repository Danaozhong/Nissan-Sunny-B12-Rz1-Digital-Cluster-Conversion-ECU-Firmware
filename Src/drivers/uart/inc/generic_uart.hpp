/*
 * generic_uart.hpp
 *
 *  Created on: 27.09.2019
 *      Author: Clemens
 */

#ifndef SRC_GENERIC_UART_HPP_
#define SRC_GENERIC_UART_HPP_

#include <cstdint>
#include <stddef.h>
#include <mutex>
#include <condition_variable>

namespace drivers
{
	/** Generic base class to handle UART communication.
	 */
	enum UARTSignalWordLength
	{
		UART_WORD_LENGTH_8BIT,
		UART_WORD_LENGTH_7BIT
	};

	enum UARTSignalStopBits
	{
		UART_STOP_BITS_1,
		UART_STOP_BITS_1_5,
		UART_STOP_BITS_2
	};

	enum UARTSignalPartity
	{
		EN_UART_PARITY_NONE,
		EN_UART_PARITY_EVEN,
		EN_UART_PARTIY_ODD
	};

	enum UARTSignalFlowControl
	{
		UART_FLOW_CONTROL_NONE,
		UART_FLOW_CONTROL_HARDWARE
	};
	class GenericUART
	{
	public:
		GenericUART();
		//GenericUART(GPIO_TypeDef* pt_rx_gpio_block,  uint16_t u16_rx_pin, GPIO_TypeDef* pt_tx_gpio_block,  uint16_t u16_tx_pin);
		virtual ~GenericUART();

	    virtual void connect(uint64_t u64_baud, UARTSignalWordLength en_word_length, UARTSignalStopBits en_stop_bits, \
	    		UARTSignalFlowControl en_flow_control, bool invert=false) = 0;
	    virtual void disconnect() = 0;
	    virtual void update_baud_rate(uint64_t u64_baud) = 0;
	    virtual int available(void) const = 0;
	    virtual int available_for_write(void) const = 0;
	    virtual int peek(void) const = 0;
	    virtual int read(void) = 0;
	    virtual void flush(void) = 0;
	    //virtual size_t write(uint8_t) = 0;
	    virtual size_t write(const uint8_t *a_u8_buffer, size_t size) = 0;

	    std::condition_variable m_cv_input_available;
	};
}

#endif /* SRC_DAC_GENERIC_DAC_HPP_ */
