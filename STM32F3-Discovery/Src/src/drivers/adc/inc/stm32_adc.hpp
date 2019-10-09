/*
 * stm32_adc.hpp
 *
 *  Created on: 28.09.2019
 *      Author: Clemens
 */

#ifndef SRC_ADC_INC_STM32_ADC_HPP_
#define SRC_ADC_INC_STM32_ADC_HPP_

#include "generic_adc.hpp"

#include "stm32f3xx_hal.h"
#include "stm32f3_discovery.h"

namespace drivers
{
	class STM32ADC : public GenericADC
	{
	public:
		/** Parameter constructor */
		STM32ADC(ADCResolution en_resolution, ADC_TypeDef* pt_adc_peripheral, uint32_t u32_adc_channel, GPIO_TypeDef* pt_gpio_block, uint16_t u16_gpio_pin);

		/** Destructor. */
		virtual ~STM32ADC();

		virtual uint32_t get_adc_min_value() const;
		virtual uint32_t get_adc_max_value() const;

		virtual uint32_t read_adc_value();
	private:
		ADC_HandleTypeDef m_adc_handle;
		ADCResolution m_en_adc_resolution;
		uint32_t m_u32_adc_channel;
		void Error_Handler(void);

		/** The GPIO where this ADC is connected to */
		GPIO_TypeDef* m_pt_gpio_block;

		/** The pin of the GPIO block m_pt_gpio_block where the ADC is connected to */
		uint16_t m_u16_gpio_pin;
	};

}


#endif /* SRC_ADC_INC_STM32_ADC_HPP_ */
