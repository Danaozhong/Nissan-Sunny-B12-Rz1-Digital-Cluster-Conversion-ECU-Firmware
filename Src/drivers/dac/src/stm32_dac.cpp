/*
 * stm32_dac.cpp
 *
 *  Created on: 27.09.2019
 *      Author: Clemens
 */


#include <algorithm>

#include "stm32_dac.hpp"

#if defined(STM32_FAMILY_F3)
#define DACx_FORCE_RESET()              __HAL_RCC_DAC1_FORCE_RESET()
#define DACx_RELEASE_RESET()            __HAL_RCC_DAC1_RELEASE_RESET()
#elif defined(STM32_FAMILY_F4)
#define DACx_FORCE_RESET()              __HAL_RCC_DAC_FORCE_RESET()
#define DACx_RELEASE_RESET()            __HAL_RCC_DAC_RELEASE_RESET()
#endif

namespace drivers
{

	STM32DAC::STM32DAC(DAC_TypeDef* pt_dac_peripheral, GPIO_TypeDef* pt_gpio_block, uint16_t u16_gpio_pin)
		: m_pt_dac_peripheral(pt_dac_peripheral)
	{
		this->m_dac_handle.Instance = m_pt_dac_peripheral;

		// Initialize the DAC GPIO peripheral
		if (GPIOA == pt_gpio_block)
		{
			__HAL_RCC_GPIOA_CLK_ENABLE();
		}
		if (DAC1 == pt_dac_peripheral)
		{
#if defined(STM32_FAMILY_F3)
		    __HAL_RCC_DAC1_CLK_ENABLE();
#elif defined(STM32_FAMILY_F4)
		    __HAL_RCC_DAC_CLK_ENABLE();
#endif
		}

		GPIO_InitTypeDef GPIO_InitStruct;
		GPIO_InitStruct.Pin = u16_gpio_pin;
		GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		HAL_GPIO_Init(pt_gpio_block, &GPIO_InitStruct);

#ifdef HAL_DAC_MODULE_ENABLED
		 if (HAL_DAC_Init(&this->m_dac_handle) != HAL_OK)
		 {
		   /* Initialization Error */
		   Error_Handler();
		 }

		 /*##-2- Configure DAC channel1 #############################################*/
		 this->m_dac_channel_config.DAC_Trigger = DAC_TRIGGER_NONE;
		 this->m_dac_channel_config.DAC_OutputBuffer = DAC_OUTPUTBUFFER_DISABLE;

		 if (HAL_DAC_ConfigChannel(&this->m_dac_handle, &this->m_dac_channel_config, get_dac_channel()) != HAL_OK)
		 {
		   /* Channel configuration Error */
		   Error_Handler();
		 }
#endif
	}

	STM32DAC::~STM32DAC()
	{
		/* Enable DAC reset state */
		DACx_FORCE_RESET();

		/* Release DAC from reset state */
		DACx_RELEASE_RESET();
	}

	/** Function to set the output by value */
	int32_t STM32DAC::set_output_value(uint32_t value)
	{
		//value = std::min(value, this->get_max_value());
		//value = std::max(value, this->get_min_value());
#ifdef HAL_DAC_MODULE_ENABLED
		/*##-3- Set DAC Channel1 DHR register ######################################*/
		if (HAL_DAC_SetValue(&this->m_dac_handle, get_dac_channel(), DAC_ALIGN_8B_R, static_cast<uint8_t>(value)) != HAL_OK)
		{
			/* Setting value Error */
			return -1;
		}

		/*##-4- Enable DAC Channel1 ################################################*/
		if (HAL_DAC_Start(&this->m_dac_handle, get_dac_channel()) != HAL_OK)
		{
			/* Start Error */
			return -2;
		}
#endif
		return 0;

	}

	uint32_t STM32DAC::get_max_value() const
	{
		return 0xFF;
	}

	uint32_t STM32DAC::get_min_value() const
	{
		return 0x0;
	}

	int32_t STM32DAC::get_max_voltage() const
	{
		return 3100; // VDDA - 0.2V
	}

	int32_t STM32DAC::get_min_voltage() const
	{
		return 200; // 0.2V
	}

	int32_t STM32DAC::set_output_voltage(int32_t i32_value)
	{
		i32_value = std::max(i32_value, static_cast<int32_t>(0));
		const uint32_t u32_adc_value = static_cast<uint32_t>(i32_value * get_max_value()) / 3300u;
		return set_output_value(u32_adc_value);
	}

	uint32_t STM32DAC::get_dac_channel() const
	{
		if (m_pt_dac_peripheral == DAC1)
		{
			return DAC_CHANNEL_1;
		}
		return DAC_CHANNEL_2;
	}

	void STM32DAC::Error_Handler(void)
	{
	  /* User may add here some code to deal with this error */
	  while(1)
	  {
#ifdef USE_STM32_F3_DISCO
		  BSP_LED_Toggle(LED_RED);
#elif defined USE_STM32F3XX_NUCLEO_32
		 // BSP_LED_Toggle(LED_GREEN);
#endif
	    HAL_Delay(1000);
	  }
	}

}


extern "C"
{
	void HAL_DAC_MspInit(DAC_HandleTypeDef *hdac)
	{
	  /*##-1- Enable peripherals and GPIO Clocks #################################*/
	  /* Enable GPIO clock ****************************************/
	  //DACx_CHANNEL_GPIO_CLK_ENABLE();
	  /* DAC Periph clock enable */
	  //DACx_CLK_ENABLE();

	  /* Do not initialize the GPIO here, do it in the constructor of the DAC class */
	}

	void HAL_DAC_MspDeInit(DAC_HandleTypeDef *hdac)
	{

	}
}
