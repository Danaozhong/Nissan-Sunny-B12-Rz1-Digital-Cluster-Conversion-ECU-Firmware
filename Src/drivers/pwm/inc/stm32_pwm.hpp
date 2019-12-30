
#ifndef _STM32_PWM_HPP_
#define _STM32_PWM_HPP_

/* System headers */
#include "stm32f3xx_hal.h"

/* Own headers */
#include "generic_pwm.hpp"

namespace drivers
{
	class STM32PWM : public GenericPWM
	{
	public:
		STM32PWM(TIM_TypeDef* pt_timer_unit, uint32_t u32_timer_channel,
				GPIO_TypeDef* pt_gpio_block, uint16_t u16_gpio_pin);
		virtual ~STM32PWM();

		virtual void set_frequency(uint32_t u32_frequency_mhz);

		virtual void set_duty_cycle(uint32_t u32_duty_cycle);

	private:
		void Error_Handler(void);
		int32_t calculate_prescaler_value(uint32_t u32_frequency_mhz) const;

#ifdef HAL_TIM_MODULE_ENABLED
		/* Timer handler declaration */
		TIM_HandleTypeDef    o_timer_handle;
		/* Timer Output Compare Configuration Structure declaration */
		TIM_OC_InitTypeDef sConfig;
		/* Counter Prescaler value */
		uint32_t u32_prescaler_value;
#endif

	};

}

#endif /* _STM32_PWM_HPP_ */
