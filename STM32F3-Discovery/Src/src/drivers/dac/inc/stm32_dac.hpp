/*
 * stm32_dac.hpp
 *
 *  Created on: 27.09.2019
 *      Author: Clemens
 */

#ifndef SRC_DAC_STM32_DAC_HPP_
#define SRC_DAC_STM32_DAC_HPP_

/* System headers */
#include "stm32f3xx_hal.h"
#include "stm32f3_discovery.h"

/* Own headers */
#include "generic_dac.hpp"

#if 0
#define DACx                            DAC1
#define DACx_CHANNEL_GPIO_CLK_ENABLE()  __HAL_RCC_GPIOA_CLK_ENABLE()

#define DACx_CLK_ENABLE()               __HAL_RCC_DAC1_CLK_ENABLE()
#define DACx_FORCE_RESET()              __HAL_RCC_DAC1_FORCE_RESET()
#define DACx_RELEASE_RESET()            __HAL_RCC_DAC1_RELEASE_RESET()

/* Definition for DACx Channel Pin */
#define DACx_CHANNEL_PIN                GPIO_PIN_4
#define DACx_CHANNEL_GPIO_PORT          GPIOA

/* Definition for DACx's Channel */
#define DACx_CHANNEL                    DAC_CHANNEL_1
#endif

namespace drivers
{
	/** Generic base class to handle the functionality of an DAC. Can be a STM32 internal DAC, or
	 * an DAC controlled by a PWM, or whatever...
	 */
	class STM32DAC : public GenericDAC
	{
	public:
		STM32DAC(DAC_TypeDef* pt_dac_peripheral, GPIO_TypeDef* pt_gpio_block, uint16_t u16_gpio_pin);
		virtual ~STM32DAC();

		/** Function to set the output by value */
		virtual int32_t set_output_value(uint32_t value);
		virtual uint32_t get_max_value() const;
		virtual uint32_t get_min_value() const;

		virtual int32_t set_output_voltage(float value);
		virtual float get_max_voltage() const;
		virtual float get_min_voltage() const;

	private:

		/** The peripheral used by this DAC (DAC1 or DAC2) */
		DAC_TypeDef* m_pt_dac_peripheral;

		/** Internal handle to the DAC. */
		DAC_HandleTypeDef m_dac_handle;

		/** Internal handle to the DAC channel configuration */
		DAC_ChannelConfTypeDef m_dac_channel_config;

		/** Helper function. Return DAC_CHANNEL_1 if DAC1, or DAC_CHANNEL_2 if DAC2 */
		uint32_t get_dac_channel() const;

		void Error_Handler(void);
	};
}

extern "C"
{
	/**
	* @brief DAC MSP Initialization
	*        This function configures the hardware resources used in this example:
	*           - Peripheral's clock enable
	*           - Peripheral's GPIO Configuration
	* @param hdac: DAC handle pointer
	* @retval None
	*/
	void HAL_DAC_MspInit(DAC_HandleTypeDef *hdac);

	/**
	* @brief  DeInitializes the DAC MSP.
	* @param  hdac: pointer to a DAC_HandleTypeDef structure that contains
	*         the configuration information for the specified DAC.
	* @retval None
	*/
	void HAL_DAC_MspDeInit(DAC_HandleTypeDef *hdac);
}



#endif /* SRC_DAC_STM32_DAC_HPP_ */
