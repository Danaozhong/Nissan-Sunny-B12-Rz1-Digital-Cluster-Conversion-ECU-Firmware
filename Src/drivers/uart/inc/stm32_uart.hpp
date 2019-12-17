/*
 * stm32_uart.hpp
 *
 *  Created on: 27.09.2019
 *      Author: Clemens
 */

#ifndef _STM32_UART_HPP_
#define _STM32_UART_HPP_

/* System headers */
#include <atomic>
#include "stm32f3xx_hal.h"
#ifdef USE_STM32_F3_DISCO
#include "stm32f3_discovery.h"
#elif defined USE_STM32F3XX_NUCLEO_32
//#include "stm32f3xx_nucleo_32.h"
#endif

/* Own headers */
#include "generic_uart.hpp"
#include "ex_thread.hpp"

#define STM32UART_BUFFER_SIZE  (8u)


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

	    void uart_main();
	private:
	    uint8_t m_au8_rx_buffer[STM32UART_BUFFER_SIZE];
	    //uint8_t m_au8_tx_buffer[STM32UART_BUFFER_SIZE];

	    uint32_t m_u32_rx_buffer_usage;

	    std_ex::thread* m_p_uart_buffer_thread;

	    std::atomic<bool> m_bo_connected;
#ifdef HAS_STD_MUTEX
	    std::mutex m_o_interrupt_mutex;
#endif

	    void Error_Handler(void) const;
	public:
	    UART_HandleTypeDef m_o_uart_handle;

	    __IO ITStatus UartReady;
	    ITStatus m_uart_rx_interrupt_status;
	};
}

extern "C"
{
	void USART2_IRQHandler(void);
}
#endif /* _STM32_UART_HPP_ */