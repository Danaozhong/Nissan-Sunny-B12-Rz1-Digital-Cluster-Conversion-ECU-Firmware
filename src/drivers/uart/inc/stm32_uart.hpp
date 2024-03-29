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
#include <mutex>

#include "hwheader.h"

/* Own headers */
#include "generic_uart.hpp"

#define STM32UART_BUFFER_SIZE  (8u) // Todo move to cmake 

// use this is you want to do the rx processing of the UART in a separate task
//#define DRIVER_UART_HAS_OWN_TASK // TODO Move to Cmake


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

        void uart_process_cycle();
#ifdef DRIVER_UART_HAS_OWN_TASK
        void uart_main();
#endif
    private:
        uint8_t m_au8_rx_buffer[STM32UART_BUFFER_SIZE];

        uint32_t m_u32_rx_buffer_usage;

        bool m_bo_initialized;
        std::mutex m_o_interrupt_mutex;

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
