/*
 * stm32_dac.cpp
 *
 *  Created on: 27.09.2019
 *      Author: Clemens
 */

#include <cstring>
#include "stm32_uart.hpp"


/* Definition for USARTx clock resources */
#define USARTx                           USART2
#define USARTx_CLK_ENABLE()              __HAL_RCC_USART2_CLK_ENABLE();
#define USARTx_RX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOD_CLK_ENABLE()
#define USARTx_TX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOD_CLK_ENABLE()

#define USARTx_FORCE_RESET()             __HAL_RCC_USART2_FORCE_RESET()
#define USARTx_RELEASE_RESET()           __HAL_RCC_USART2_RELEASE_RESET()



namespace drivers
{
	STM32HardwareUART::STM32HardwareUART(GPIO_TypeDef* pt_rx_gpio_block,  uint16_t u16_rx_pin, \
			GPIO_TypeDef* pt_tx_gpio_block,  uint16_t u16_tx_pin)
	{
		m_o_uart_handle = {};

		// Configure pins
		GPIO_InitTypeDef  GPIO_InitStruct;

		if (pt_rx_gpio_block == GPIOD && u16_rx_pin == GPIO_PIN_6 &&
			pt_tx_gpio_block == GPIOD && u16_tx_pin == GPIO_PIN_5)
			{
			/*##-1- Enable peripherals and GPIO Clocks #################################*/
			/* Enable GPIO TX/RX clock */
			__HAL_RCC_GPIOD_CLK_ENABLE();

			/* Enable USARTx clock */
			__HAL_RCC_USART2_CLK_ENABLE();

			/*##-2- Configure peripheral GPIO ##########################################*/
			/* UART TX GPIO pin configuration  */
			GPIO_InitStruct.Pin       = u16_tx_pin;
			GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
			GPIO_InitStruct.Pull      = GPIO_PULLUP;
			GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_HIGH;
			GPIO_InitStruct.Alternate = GPIO_AF7_USART2;

			HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

			/* UART RX GPIO pin configuration  */
			GPIO_InitStruct.Pin = u16_rx_pin;
			GPIO_InitStruct.Alternate = GPIO_AF7_USART2;

			HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);


			// STM32F303 UART2 peripherals
			m_o_uart_handle.Instance        = USART2;
		}
	}

	STM32HardwareUART::~STM32HardwareUART()
	{}


	void STM32HardwareUART::connect(uint64_t u64_baud, UARTSignalWordLength en_word_length, UARTSignalStopBits en_stop_bits, \
			UARTSignalFlowControl en_flow_control, bool invert)
	{
	  /*##-1- Configure the UART peripheral ######################################*/
	  /* Put the USART peripheral in the Asynchronous mode (UART Mode) */
	  /* UART configured as follows:
	      - Word Length = 8 Bits
	      - Stop Bit = One Stop bit
	      - Parity = None
	      - BaudRate = 9600 baud
	      - Hardware flow control disabled (RTS and CTS signals) */

		m_o_uart_handle.Init.BaudRate = u64_baud;
		switch (en_word_length)
		{
		case UART_WORD_LENGTH_8BIT:
			m_o_uart_handle.Init.WordLength = UART_WORDLENGTH_8B;
		break;
		}

		switch (en_stop_bits)
		{
		case UART_STOP_BITS_1_5:
			m_o_uart_handle.Init.StopBits     = UART_STOPBITS_1_5;
			break;
		case UART_STOP_BITS_1:
			m_o_uart_handle.Init.StopBits     = UART_STOPBITS_1;
			break;
		}

		switch (en_flow_control)
		{
		case UART_FLOW_CONTROL_NONE:
			m_o_uart_handle.Init.Parity       = UART_PARITY_NONE;
			break;
		}

		m_o_uart_handle.Init.Mode         = UART_MODE_TX_RX;
		m_o_uart_handle.Init.OverSampling = UART_OVERSAMPLING_16;
		m_o_uart_handle.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;



		if(HAL_UART_DeInit(&m_o_uart_handle) != HAL_OK)
		{
			Error_Handler();
		}
		if(HAL_UART_Init(&m_o_uart_handle) != HAL_OK)
		{
			Error_Handler();
		}
	}
	void STM32HardwareUART::disconnect()
	{

	}

	void STM32HardwareUART::update_baud_rate(uint64_t u64_baud)
	{

	}

	int STM32HardwareUART::available(void) const
	{
		return 0;
	}

	int STM32HardwareUART::available_for_write(void) const
	{
		return 0;
	}

	int STM32HardwareUART::peek(void) const
	{
		return 0;
	}

	int STM32HardwareUART::read(void)
	{
		char ai8_buffer[10];


		if(HAL_UART_Receive(&m_o_uart_handle, reinterpret_cast<uint8_t*>(ai8_buffer), 1, 1) != HAL_OK)
		{
			return -1;
		}
		return static_cast<int>(ai8_buffer[0]);
	}

	void STM32HardwareUART::flush(void)
	{
	}

	size_t STM32HardwareUART::write(const uint8_t *a_u8_buffer, size_t size)
	{
		if(HAL_UART_Transmit(&m_o_uart_handle, const_cast<uint8_t*>(a_u8_buffer), size, 5000) != HAL_OK)
		{
			Error_Handler();
		}
		return size;
	}
	void STM32HardwareUART::Error_Handler(void) const
	{
	  /* User may add here some code to deal with this error */
	  while(1)
	  {
	    BSP_LED_Toggle(LED_RED);
	    HAL_Delay(1000);
	  }
	}
}

#if 0
extern "C"
{
	void HAL_UART_MspInit(UART_HandleTypeDef *huart)
	{
	  GPIO_InitTypeDef  GPIO_InitStruct;

	  /*##-1- Enable peripherals and GPIO Clocks #################################*/
	  /* Enable GPIO TX/RX clock */
	  USARTx_TX_GPIO_CLK_ENABLE();
	  USARTx_RX_GPIO_CLK_ENABLE();


	  /* Enable USARTx clock */
	  USARTx_CLK_ENABLE();

	  /*##-2- Configure peripheral GPIO ##########################################*/
	  /* UART TX GPIO pin configuration  */
	  GPIO_InitStruct.Pin       = USARTx_TX_PIN;
	  GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
	  GPIO_InitStruct.Pull      = GPIO_PULLUP;
	  GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_HIGH;
	  GPIO_InitStruct.Alternate = USARTx_TX_AF;

	  HAL_GPIO_Init(USARTx_TX_GPIO_PORT, &GPIO_InitStruct);

	  /* UART RX GPIO pin configuration  */
	  GPIO_InitStruct.Pin = USARTx_RX_PIN;
	  GPIO_InitStruct.Alternate = USARTx_RX_AF;

	  HAL_GPIO_Init(USARTx_RX_GPIO_PORT, &GPIO_InitStruct);
	}

	/**
	  * @brief UART MSP De-Initialization
	  *        This function frees the hardware resources used in this example:
	  *          - Disable the Peripheral's clock
	  *          - Revert GPIO configuration to their default state
	  * @param huart: UART handle pointer
	  * @retval None
	  */
	void HAL_UART_MspDeInit(UART_HandleTypeDef *huart)
	{
	  /*##-1- Reset peripherals ##################################################*/
	  USARTx_FORCE_RESET();
	  USARTx_RELEASE_RESET();

	  /*##-2- Disable peripherals and GPIO Clocks #################################*/
	  /* Configure USART2 Tx as alternate function  */
	  HAL_GPIO_DeInit(USARTx_TX_GPIO_PORT, USARTx_TX_PIN);
	  /* Configure USART2 Rx as alternate function  */
	  HAL_GPIO_DeInit(USARTx_RX_GPIO_PORT, USARTx_RX_PIN);
	}
}
#endif

