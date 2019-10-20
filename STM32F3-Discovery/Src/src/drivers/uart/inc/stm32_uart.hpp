/*
 * stm32_uart.hpp
 *
 *  Created on: 27.09.2019
 *      Author: Clemens
 */

#ifndef _STM32_UART_HPP_
#define _STM32_UART_HPP_

/* System headers */
#include "stm32f3xx_hal.h"
#include "stm32f3_discovery.h"

/* Own headers */
#include "generic_uart.hpp"

namespace drivers
{
class STM32HardwareUART : public GenericUART
	{
	public:
		STM32HardwareUART(GPIO_TypeDef* pt_rx_gpio_block, uint16_t u16_rx_pin, \
				GPIO_TypeDef* pt_tx_gpio_block,  uint16_t u16_tx_pin);
		virtual ~STM32HardwareUART();

	    virtual void connect(uint64_t u64_baud, UARTSignalWordLength en_word_length, UARTSignalStopBits en_stop_bits, \
	    		UARTSignalFlowControl en_flow_control, bool invert=false);
	    virtual void disconnect();
	    virtual void update_baud_rate(uint64_t u64_baud);
	    virtual int available(void) const;
	    virtual int available_for_write(void) const;
	    virtual int peek(void) const;
	    virtual int read(void);
	    virtual void flush(void);
	    virtual size_t write(const uint8_t *a_u8_buffer, size_t size);
	private:

	    void Error_Handler(void) const;

	    UART_HandleTypeDef m_o_uart_handle;
	};
}

#endif /* _STM32_UART_HPP_ */
